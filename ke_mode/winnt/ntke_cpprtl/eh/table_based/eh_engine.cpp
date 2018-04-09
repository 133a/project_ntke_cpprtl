/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"

#include "eh_msvc_internal_data.h"
#include "eh_msvc_internal_data_aux.h"
#include "eh_engine.h"
#include "eh_engine_defs.h"
#include "eh_stack_walker.h"
#include "eh_aux.h"
#include "eh_exception_code.h"
#include "eh_invoke_funclet.h"


namespace cpprtl
{
namespace eh
{


namespace eh_engine
{
  typedef int ehstate_t;
}


namespace eh_state
{

  using eh_engine::ehstate_t;
  using eh_engine::frame_ptr_t;
  using eh_engine::funcframe_ptr_t;


  enum
  {
    INVALID   = -2,
    EMPTY     = -1,
  };



  ehstate_t from_ip
  (
    func_descriptor_iterator  const&  func_dsc
  , ::size_t                  const&  ip
  , ::DISPATCHER_CONTEXT      const&  dc
  )
  {
    ehstate_t state = EMPTY;
    rva_t ip_rva = static_cast<rva_t>(ip - dc.ImageBase);

    ip2state_iterator ip2state(*func_dsc, dc.ImageBase);
    for ( ; ip2state.valid(); ip2state.next() )
    {
      if (ip_rva < ip2state->ip)
      {
        break;
      }
    }
    if ( ip2state.prev() )
    {
      state = ip2state->state;
    }
    return state;
  }


  ehstate_t from_dc
  (
    func_descriptor_iterator  const&  func_dsc
  , ::DISPATCHER_CONTEXT      const&  dc
  )
  {
    ::size_t pc = dc.ControlPc;
  #if defined (_M_ARM)
    if ( dc.ControlPcIsUnwound )
    {
      pc -= 2;  //  rewind the forwarded execution point
    }
  #elif defined (_M_ARM64)
    if ( dc.ControlPcIsUnwound )
    {
      pc -= 4;  //  rewind the forwarded execution point
    }
  #endif
    return from_ip(func_dsc, pc, dc);
  }


  void try_range
  (
    func_descriptor_iterator  const&  func_dsc
  , ehstate_t                 const&  state
  , try_iterator                   &  try_begin
  , try_iterator                   &  try_end
  , ::DISPATCHER_CONTEXT      const&  dc
  )
  {
    ehstate_t const cs = from_dc(func_dsc, dc);
    msvc_internal_data::eh::try_descriptor const* in_catch = 0;
    for ( try_rev_iterator try_dsc(*func_dsc, dc.ImageBase); try_dsc.valid(); try_dsc.next() )
    {
      if ( cs > try_dsc->high_level && cs <= try_dsc->catch_level )
      {
        in_catch = *try_dsc;
        break;
      }
    }

    try_begin.deface();
    try_end.deface();

    for ( try_iterator try_dsc(*func_dsc, dc.ImageBase); try_dsc.valid(); try_dsc.next() )
    {
      if
      (
        in_catch
      &&
        ( try_dsc->low_level <= in_catch->high_level || try_dsc->high_level > in_catch->catch_level )
      )
      {
        continue;
      }

      if
      (
        state >= try_dsc->low_level
      &&
        state <= try_dsc->high_level
      )
      {
        if ( !try_begin.valid() )
        {
          try_begin = try_dsc;
        }
        try_end = try_dsc;
      }
    }
    if ( try_end.valid() )
    {
      try_end.next();
    }
  }


  funcframe_ptr_t function_frame
  (
    func_descriptor_iterator  const&  func_dsc
  , frame_ptr_t               const&  frame
  , ::DISPATCHER_CONTEXT      const&  dc
  )
  {
    ehstate_t state = from_dc(func_dsc, dc);
    ::size_t func_frame = reinterpret_cast< ::size_t>(frame);
    for ( try_rev_iterator try_dsc(*func_dsc, dc.ImageBase); try_dsc.valid(); try_dsc.next() )
    {
      if
      (
        state > try_dsc->high_level
      &&
        state <= try_dsc->catch_level
      )
      {
        image_base_t img_base;
        IRQL_CHECK ( <=DISPATCH_LEVEL ) //  ::RtlLookupFunctionEntry()
        ::RUNTIME_FUNCTION const* prf = ::RtlLookupFunctionEntry(dc.ControlPc, &img_base, 0);
        catch_iterator catch_block(*try_dsc, dc.ImageBase);
        while ( catch_block.valid() && prf->BeginAddress != catch_block->handler_address )
        {
          catch_block.next();
        }
        if ( catch_block.valid() )
        {
        #if defined (_M_AMD64) || defined (_M_X64)
          func_frame = *reinterpret_cast< ::size_t*>(func_frame + catch_block->frame_offset);
        #elif defined (_M_ARM) || defined (_M_ARM64)
          func_frame = *reinterpret_cast< ::size_t*>(func_frame);
        #else
        #  error check $(target.arch)
        #endif
          break;
        }
      }
    }
    return reinterpret_cast<funcframe_ptr_t>(func_frame);
  }


  namespace aux_
  {
    struct FRAME_POINTERS
    {
      int fp1;    // contains the current function state
      int fp2;

      static FRAME_POINTERS* get
      (
        func_descriptor_iterator  const&  func_dsc
      , funcframe_ptr_t           const&  func_frame
      )
      {
        return reinterpret_cast<eh_state::aux_::FRAME_POINTERS*>(reinterpret_cast< ::size_t>(func_frame) + func_dsc->frame_ptrs);
      }
    };
  }


  ehstate_t saved_state
  (
    func_descriptor_iterator  const&  func_dsc
  , funcframe_ptr_t           const&  func_frame
  )
  {
    return static_cast<ehstate_t>(aux_::FRAME_POINTERS::get(func_dsc, func_frame)->fp1);
  }


  void save_state
  (
    func_descriptor_iterator  const&  func_dsc
  , funcframe_ptr_t           const&  func_frame
  , ehstate_t                 const&  state
  )
  {
    aux_::FRAME_POINTERS::get(func_dsc, func_frame)->fp1 = state;
  }


  ehstate_t saved_unwind_try_block
  (
    func_descriptor_iterator  const&  func_dsc
  , funcframe_ptr_t           const&  func_frame
  )
  {
    return static_cast<ehstate_t>(aux_::FRAME_POINTERS::get(func_dsc, func_frame)->fp2);
  }


  void save_unwind_try_block
  (
    func_descriptor_iterator  const&  func_dsc
  , funcframe_ptr_t           const&  func_frame
  , ehstate_t                 const&  state
  )
  {
    aux_::FRAME_POINTERS::get(func_dsc, func_frame)->fp2 = state;
  }

}  // namespace eh_state



namespace eh_type
{

  using eh_engine::funcframe_ptr_t;


  msvc_internal_data::eh::exception_descriptor const* get_exception_descriptor(::EXCEPTION_RECORD const& exc_rec)
  {
    if ( eh_engine::EXCEPTION_OPCODE_THROW == exc_rec.ExceptionInformation[eh_engine::EXCPTR_OPCODE] )
    {
      return reinterpret_cast<msvc_internal_data::eh::exception_descriptor*>(exc_rec.ExceptionInformation[eh_engine::EXCPTR_THR_THROWINFO]);
    }
    return 0;
  }


  void* get_exception_object(::EXCEPTION_RECORD const& exc_rec)
  {
    if ( eh_engine::EXCEPTION_OPCODE_THROW == exc_rec.ExceptionInformation[eh_engine::EXCPTR_OPCODE] )
    {
      return reinterpret_cast<void*>(exc_rec.ExceptionInformation[eh_engine::EXCPTR_THR_THROWOBJECT]);
    }
    return 0;
  }


  bool match
  (
    msvc_internal_data::eh::exception_descriptor  const&  exc_dsc
  , catchable_type_iterator                       const&  catchable_type
  , catchable_typeinfo_iterator                   const&  catchable_typeinfo
  , catch_typeinfo_iterator                       const&  catch_typeinfo
  , unsigned                                      const&  catch_attr
  )
  {
    if
    (
      0 == *catch_typeinfo
    ||
      0 == catch_typeinfo->name
    )  // (...) check
    {
      return true;
    }

    if
    (
      *catchable_typeinfo != *catch_typeinfo
    &&
      !aux_::strzcmp(&catchable_typeinfo->name, &catch_typeinfo->name)
    )  // type_info equality check
    {
      return false;
    }

    if 
    (
      ( (catchable_type->attributes & msvc_internal_data::eh::EXCEP_REFERENCE)  &&  !(catch_attr & msvc_internal_data::eh::CATCH_REFERENCE) )
    ||
      ( (exc_dsc.attributes & msvc_internal_data::eh::EXCEP_CONST)              &&  !(catch_attr & msvc_internal_data::eh::CATCH_CONST)     )
    ||
      ( (exc_dsc.attributes & msvc_internal_data::eh::EXCEP_VOLATILE)           &&  !(catch_attr & msvc_internal_data::eh::CATCH_VOLATILE)  )
    )
    {
      return false;
    }
    return true;
  }


  void* pointer_cast
  (
    void                                       const* const  complete_obj
  , msvc_internal_data::eh::subtype_cast_info  const&        cast_info
  )
  {
    ::size_t ptr = reinterpret_cast< ::size_t>(complete_obj);
    if ( cast_info.vbase_table_offset >= 0 )
    {
      ptr += cast_info.vbase_table_offset;
      ptr += *reinterpret_cast<int*>(*reinterpret_cast< ::size_t*>(ptr) + cast_info.vbase_disp_offset);
    }
    ptr += cast_info.subtype_offset;
    return reinterpret_cast<void*>(ptr);
  }



  void copy_exception_object
  (
    void                     const* const  exc_object
  , funcframe_ptr_t          const&        func_frame
  , catch_iterator           const&        catch_block
  , catchable_type_iterator  const&        catchable_type
  , ::DISPATCHER_CONTEXT     const&        dc
  )
  {
    if ( exc_object && catch_block->exc_offset )
    {
      __try
      {
        catch_typeinfo_iterator type_dsc(*catch_block, dc.ImageBase);
        if ( type_dsc.valid() && type_dsc->name )
        {
          ::size_t dst_addr = reinterpret_cast< ::size_t>(func_frame) + catch_block->exc_offset;
  
          if ( catch_block->attributes & msvc_internal_data::eh::CATCH_REFERENCE )
          {
            *reinterpret_cast<void**>(dst_addr) = pointer_cast(exc_object, catchable_type->cast_info);
          }
          else if ( catchable_type->attributes & msvc_internal_data::eh::EXCEP_SIMPLE_TYPE )
          {
            aux_::memcpy(reinterpret_cast<void*>(dst_addr), exc_object, catchable_type->size);
            if ( sizeof(void*) == catchable_type->size )
            {
              *reinterpret_cast<void**>(dst_addr) = pointer_cast(*reinterpret_cast<void**>(dst_addr), catchable_type->cast_info);
            }
          }
          else // UDT
          {
            void* casted_exc_object = pointer_cast(exc_object, catchable_type->cast_info);
            if ( !catchable_type->cctor )
            {
              aux_::memcpy(reinterpret_cast<void*>(dst_addr), casted_exc_object, catchable_type->size);
            }
            else
            {
              if ( catchable_type->attributes & msvc_internal_data::eh::EXCEP_VIRTUAL_BASE )
              {
                (*cctorvb_iterator(*catchable_type, dc.ImageBase))(reinterpret_cast<void*>(dst_addr), casted_exc_object, 1);
              }
              else
              {
                (*cctor_iterator(*catchable_type, dc.ImageBase))(reinterpret_cast<void*>(dst_addr), casted_exc_object);
              }
            }
          }
        }
      }
      __except ( eh::aux_::invalid_exception(GetExceptionCode(), eh::EXCEPTION_SUBCODE_CCTOR_THROW) , EXCEPTION_CONTINUE_SEARCH )
      {
      }
    }
  }



  void destroy_exception_object(::EXCEPTION_RECORD const& exc_rec)
  {
    if
    (
      eh::EXCEPTION_CODE_CPP == exc_rec.ExceptionCode
    &&
      eh_engine::EXCEPTION_OPCODE_THROW == exc_rec.ExceptionInformation[eh_engine::EXCPTR_OPCODE]
    )
    {
      msvc_internal_data::eh::exception_descriptor const* const exc_dsc = eh_type::get_exception_descriptor(exc_rec);
      void* const exc_object = eh_type::get_exception_object(exc_rec);
      if ( exc_object && exc_dsc )
      {
        dtor_iterator dtor(exc_dsc, exc_rec.ExceptionInformation[eh_engine::EXCPTR_THR_IMAGEBASE]);
        if ( dtor.valid() )
        {
          __try
          {
            (*dtor)(exc_object);
          }
          __except ( eh::aux_::invalid_exception(GetExceptionCode(), eh::EXCEPTION_SUBCODE_DTOR_THROW) , EXCEPTION_CONTINUE_SEARCH )
          {
          }
        }
      }
    }
  }

}  // namespace eh_type



namespace eh_engine
{


  void unwind_frame
  (
    func_descriptor_iterator  const&  func_dsc
  , funcframe_ptr_t           const&  func_frame
  , ehstate_t                 const&  target_state
  , ::DISPATCHER_CONTEXT      const&  dc
  )
  {
    ehstate_t current_state = eh_state::saved_state(func_dsc, func_frame);
    if ( eh_state::INVALID == current_state )
    {
      current_state = eh_state::from_dc(func_dsc, dc);
    }

    unwind_iterator unwind_entry(*func_dsc, dc.ImageBase);
    while
    (
      current_state > eh_state::EMPTY
    &&
      current_state != target_state
    &&
      unwind_entry[current_state].valid()
    )
    {
      current_state = unwind_entry->prev_state;
      unwind_action_iterator unwind_action(*unwind_entry, dc.ImageBase);
      if ( unwind_action.valid() )
      {
        eh_state::save_state(func_dsc, func_frame, current_state);
        __try
        {
        #if defined (_M_X64) || defined (_M_AMD64)
          _CPPRTL_invoke_funclet(*unwind_action, func_frame);
        #elif defined (_M_ARM) || defined (_M_ARM64)
          _CPPRTL_invoke_funclet(*unwind_action, func_frame, dc.NonVolatileRegisters);
        #else
        #  error check $(target.arch)
        #endif
        }
        __except ( eh::aux_::invalid_exception(GetExceptionCode(), eh::EXCEPTION_SUBCODE_UNWIND_THROW) , EXCEPTION_CONTINUE_SEARCH )
        {
        }
      }
    }
    eh_state::save_state(func_dsc, func_frame, current_state);
  }



  namespace aux_
  {

    int call_catch_block_rethrow_seh_filter
    (
      ::EXCEPTION_POINTERS  *        xp
    , ::EXCEPTION_RECORD    * const  cur_exc
    )
    {
      if ( eh::EXCEPTION_CODE_CPP == xp->ExceptionRecord->ExceptionCode )
      {
        ::EXCEPTION_RECORD* new_exc = xp->ExceptionRecord;
        if ( new_exc )
        {

          if ( EXCEPTION_OPCODE_THROW == new_exc->ExceptionInformation[EXCPTR_OPCODE] )
          {
            if ( 0 == new_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT] )
            {
              new_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT]      = cur_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT];
              new_exc->ExceptionInformation[EXCPTR_THR_THROWINFO]        = cur_exc->ExceptionInformation[EXCPTR_THR_THROWINFO];
              new_exc->ExceptionInformation[EXCPTR_THR_IMAGEBASE]        = cur_exc->ExceptionInformation[EXCPTR_THR_IMAGEBASE];
              new_exc->ExceptionInformation[EXCPTR_THR_PREV_EXCEPTION]   = reinterpret_cast< ::ULONG_PTR>(cur_exc);
            // our exception object is propagated to the outer scope so we are no longer responsible for its' destruction.
              cur_exc->ExceptionInformation[EXCPTR_FLAGS]           |= EXCEPTION_FLAG_OBJECT_RETHROWED;
            // delegate the exception object's destruction to the outer scope's catch-block caller finally handler
              new_exc->ExceptionInformation[EXCPTR_FLAGS]           &= ~EXCEPTION_FLAG_OBJECT_RETHROWED;
            }
            else if (cur_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT] == new_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT])
            {
              new_exc->ExceptionInformation[EXCPTR_THR_PREV_EXCEPTION]   = reinterpret_cast< ::ULONG_PTR>(cur_exc);
              new_exc->ExceptionInformation[EXCPTR_THR_IMAGEBASE]        = cur_exc->ExceptionInformation[EXCPTR_THR_IMAGEBASE];
            // our exception object is propagated to the outer scope so we are no longer responsible for its' destruction.
              cur_exc->ExceptionInformation[EXCPTR_FLAGS]           |= EXCEPTION_FLAG_OBJECT_RETHROWED;
            // delegate the exception object's destruction to the outer scope's catch-block caller finally handler
              new_exc->ExceptionInformation[EXCPTR_FLAGS]           &= ~EXCEPTION_FLAG_OBJECT_RETHROWED;
            }

            return EXCEPTION_CONTINUE_SEARCH;
          }

          if ( EXCEPTION_OPCODE_NO_EXC_OBJ == new_exc->ExceptionInformation[EXCPTR_OPCODE] )
          {
            ::EXCEPTION_RECORD* const rec_patch = reinterpret_cast< ::EXCEPTION_RECORD*>(new_exc->ExceptionInformation[EXCPTR_NOOBJ_EXCREC_PTR]);
            if ( rec_patch && EXCEPTION_OPCODE_THROW == rec_patch->ExceptionInformation[EXCPTR_OPCODE] )
            {
              rec_patch->ExceptionInformation[EXCPTR_THR_THROWOBJECT]      = cur_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT];
              rec_patch->ExceptionInformation[EXCPTR_THR_THROWINFO]        = cur_exc->ExceptionInformation[EXCPTR_THR_THROWINFO];
              rec_patch->ExceptionInformation[EXCPTR_THR_IMAGEBASE]        = cur_exc->ExceptionInformation[EXCPTR_THR_IMAGEBASE];
              rec_patch->ExceptionInformation[EXCPTR_THR_PREV_EXCEPTION]   = reinterpret_cast< ::ULONG_PTR>(cur_exc);
            // put the flag that our scope (and may be outer ones) is responsible of the exception object's lifetime
              rec_patch->ExceptionInformation[EXCPTR_FLAGS]           |= EXCEPTION_FLAG_OBJECT_RETHROWED;
            }
            return EXCEPTION_CONTINUE_EXECUTION;
          }
        }
      }
      return EXCEPTION_CONTINUE_SEARCH;
    }

  }  // namespace aux_



  void* call_catch_block(::EXCEPTION_RECORD const& unwind_exc_rec)
  {
    ::EXCEPTION_RECORD& cur_exc = *reinterpret_cast< ::EXCEPTION_RECORD*>(unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_CURRENT_EXCEPTION]);

    void* ret_addr = 0;
    __try
    {
      __try
      {
        ret_addr =
      #if defined (_M_X64) || defined (_M_AMD64)
          _CPPRTL_invoke_funclet
          (
            reinterpret_cast<msvc_internal_data::eh::catch_handler_ft> (unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_HANDLER_ADDR])
          , reinterpret_cast<void*>                                    (unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_FUNCTION_FRAME])
          );
      #elif defined (_M_ARM) || defined (_M_ARM64)
          _CPPRTL_invoke_funclet
          (
            reinterpret_cast<msvc_internal_data::eh::catch_handler_ft> (unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_HANDLER_ADDR])
          , reinterpret_cast<void*>                                    (unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_FUNCTION_FRAME])
          , reinterpret_cast<void*>                                    (unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_ARM_NV_CONTEXT])
          );
      #else
      #  error check $(target.arch)
      #endif
      }
      __except ( aux_::call_catch_block_rethrow_seh_filter(GetExceptionInformation(), &cur_exc) )
      {
      }
    }
    __finally
    {
      if ( !(cur_exc.ExceptionInformation[EXCPTR_FLAGS] & EXCEPTION_FLAG_OBJECT_RETHROWED) )
      {
        eh_type::destroy_exception_object(cur_exc);
      }
    }

    eh_state::save_unwind_try_block
    (
      func_descriptor_iterator(reinterpret_cast<msvc_internal_data::eh::func_descriptor*>(unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_FUNC_DESCRIPTOR]))
    , reinterpret_cast<funcframe_ptr_t>(unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_FUNCTION_FRAME])
    , eh_state::INVALID
    );

    return ret_addr;
  }


  namespace aux_
  {
    void init_unwind_exception_record
    (
      ::EXCEPTION_RECORD               &  unwind_exc_rec
    , ::EXCEPTION_RECORD          const&  exc_rec
    , func_descriptor_iterator    const&  func_dsc
    , funcframe_ptr_t             const&  func_frame
    , frame_ptr_t                 const&  target_frame
    , ehstate_t                   const&  target_state
    , catch_handler_iterator      const&  handler
    )
    {
      unwind_exc_rec.ExceptionCode     = STATUS_UNWIND_CONSOLIDATE;
      unwind_exc_rec.ExceptionFlags   |= EXCEPTION_NONCONTINUABLE;
      unwind_exc_rec.ExceptionRecord   = 0;
      unwind_exc_rec.ExceptionAddress  = 0;
    
      unwind_exc_rec.ExceptionInformation[EXCPTR_OPCODE]                   = EXCEPTION_OPCODE_STACK_CONSOLIDATE;                     // operation sub-code
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_CALLBACK_ADDR]        = reinterpret_cast< ::ULONG_PTR>(&call_catch_block);      // Address of callback function
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_FUNCTION_FRAME]       = reinterpret_cast< ::ULONG_PTR>(func_frame);             // Used by callback funciton
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_HANDLER_ADDR]         = reinterpret_cast< ::ULONG_PTR>(*handler);               // Used by callback function to call catch block
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_TARGET_STATE]         = target_state;                                           // Used by CxxFrameHandler to unwind to target_state
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_TARGET_FRAME]         = reinterpret_cast< ::ULONG_PTR>(target_frame);           // used by CFG_EH_STACK_WALKER
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_FUNC_DESCRIPTOR]      = reinterpret_cast< ::ULONG_PTR>(*func_dsc);              // Used in callback function to set state on stack to -2
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_CURRENT_EXCEPTION]    = reinterpret_cast< ::ULONG_PTR>(&exc_rec);               // Used for passing current Exception
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_ARM_NV_CONTEXT]       = -1;                                                     // _M_ARM specific
    
      unwind_exc_rec.NumberParameters = ARRAYSIZE_EXCPTR_UNW;
    }

    void unwind_stack_helper
    (
      ::EXCEPTION_RECORD          const&  exc_rec
    , func_descriptor_iterator    const&  func_dsc
    , funcframe_ptr_t             const&  func_frame
    , frame_ptr_t                 const&  target_frame
    , ehstate_t                   const&  target_state
    , catch_handler_iterator      const&  handler
    , ::DISPATCHER_CONTEXT        const&  dc
    )
    {
      ::EXCEPTION_RECORD unwind_exc_rec = { 0 };
      init_unwind_exception_record
      (
        unwind_exc_rec
      , exc_rec
      , func_dsc
      , func_frame
      , target_frame
      , target_state
      , handler
      );
      ::RtlUnwindEx(target_frame, reinterpret_cast<void*>(dc.ControlPc), &unwind_exc_rec, 0, dc.ContextRecord, dc.HistoryTable);
    }
  }  // namespace aux_


  void unwind_stack
  (
    ::EXCEPTION_RECORD          const&  exc_rec
  , func_descriptor_iterator    const&  func_dsc
  , funcframe_ptr_t             const&  func_frame
  , frame_ptr_t                 const&  target_frame
  , ehstate_t                   const&  target_state
  , catch_handler_iterator      const&  handler
  , ::DISPATCHER_CONTEXT        const&  dc
  )
  {
    if
    (
      eh::EXCEPTION_CODE_CPP == exc_rec.ExceptionCode
    &&
      EXCEPTION_OPCODE_THROW == exc_rec.ExceptionInformation[EXCPTR_OPCODE]
    )
    {
      if ( exc_rec.ExceptionInformation[EXCPTR_FLAGS] & EXCEPTION_FLAG_STACKWALKER_UNWIND )
      {
        ::EXCEPTION_RECORD& unwind_exc_rec = *reinterpret_cast< ::EXCEPTION_RECORD*>(exc_rec.ExceptionInformation[EXCPTR_THR_UNWIND_EXCREC]);
        aux_::init_unwind_exception_record
        (
          unwind_exc_rec
        , exc_rec
        , func_dsc
        , func_frame
        , target_frame
        , target_state
        , handler
        );
        return;  // back into the stack walker
      }
    }
    aux_::unwind_stack_helper
    (
      exc_rec
    , func_dsc
    , func_frame
    , target_frame
    , target_state
    , handler
    , dc
    );
  }


  void throw_exception_no_exception_object(::EXCEPTION_RECORD const* const rec)
  {
    ::EXCEPTION_RECORD exc_rec  = { 0 };
    exc_rec.ExceptionCode       = eh::EXCEPTION_CODE_CPP;
    exc_rec.ExceptionFlags      = 0;  // this is a continuable exception - if catchblock-handler would fill the necessary ptrs it will return ExceptionContinueExecution
    exc_rec.ExceptionRecord     = 0;
    exc_rec.ExceptionAddress    = 0;
    exc_rec.ExceptionInformation[EXCPTR_OPCODE]            = EXCEPTION_OPCODE_NO_EXC_OBJ;
    exc_rec.ExceptionInformation[EXCPTR_NOOBJ_EXCREC_PTR]  = reinterpret_cast< ::ULONG_PTR>(rec);
    exc_rec.NumberParameters = ARRAYSIZE_EXCPTR_NOOBJ;

  #ifdef CFG_EH_STACK_WALKER
    eh::eh_engine::stack_walk(exc_rec);
  #else
    eh::aux_::raise_exception(exc_rec);
  #endif

  }


  void throw_exception
  (
    void                                          const* const  exc_object
  , msvc_internal_data::eh::exception_descriptor  const* const  exc_descr
  )
  {
    ::EXCEPTION_RECORD exc_rec  = { 0 };
    exc_rec.ExceptionCode       = eh::EXCEPTION_CODE_CPP;
    exc_rec.ExceptionFlags     |= EXCEPTION_NONCONTINUABLE;
    exc_rec.ExceptionRecord     = 0;
    exc_rec.ExceptionAddress    = 0;
    exc_rec.ExceptionInformation[EXCPTR_OPCODE]           = EXCEPTION_OPCODE_THROW;
    exc_rec.ExceptionInformation[EXCPTR_THR_THROWOBJECT]  = reinterpret_cast< ::ULONG_PTR>(exc_object);
    exc_rec.ExceptionInformation[EXCPTR_THR_THROWINFO]    = reinterpret_cast< ::ULONG_PTR>(exc_descr);
    if ( exc_descr )
    {
      IRQL_CHECK ( <=DISPATCH_LEVEL )  //  RtlPcToFileHeader()
      exc_rec.ExceptionInformation[EXCPTR_THR_IMAGEBASE]  = 
        reinterpret_cast< ::ULONG_PTR>
        (
          ::RtlPcToFileHeader
          (
            reinterpret_cast<void*>  (const_cast<msvc_internal_data::eh::exception_descriptor*>(exc_descr))
          , reinterpret_cast<void**> (&exc_rec.ExceptionInformation[EXCPTR_THR_IMAGEBASE])
          )
        );
    }
    exc_rec.NumberParameters = ARRAYSIZE_EXCPTR_THROW;

    if ( !exc_object || !exc_descr )
    {
      // if we are here the 'throw;' statement has occured and we are to try getting the exception info from the previous scope's rethrow-seh-filter by continuable seh-exception
      throw_exception_no_exception_object(&exc_rec);
    }

  #ifdef CFG_EH_STACK_WALKER
    eh::eh_engine::stack_walk(exc_rec);
  #else
    eh::aux_::raise_exception(exc_rec);
  #endif
  }



  void find_matching_catch_block
  (
    ::EXCEPTION_RECORD        const&  exc_rec
  , func_descriptor_iterator  const&  func_dsc
  , frame_ptr_t               const&  frame
  , ::DISPATCHER_CONTEXT      const&  dc
  )
  {
    funcframe_ptr_t const func_frame = eh_state::function_frame(func_dsc, frame, dc);

    ehstate_t state = eh_state::from_dc(func_dsc, dc);
    if ( state > eh_state::saved_unwind_try_block(func_dsc, func_frame) )
    {                             
      eh_state::save_state(func_dsc, func_frame, state);
      eh_state::save_unwind_try_block(func_dsc, func_frame, state);
    }
    else
    {
      state = eh_state::saved_unwind_try_block(func_dsc, func_frame);
    }

    try_iterator try_cur(*func_dsc, dc.ImageBase);
    try_iterator try_end(*func_dsc, dc.ImageBase);
    eh_state::try_range(func_dsc, state, try_cur, try_end, dc);

    if ( eh::EXCEPTION_CODE_CPP == exc_rec.ExceptionCode )  // ...for cpp-exception
    {
      if ( EXCEPTION_OPCODE_THROW == exc_rec.ExceptionInformation[EXCPTR_OPCODE] )
      {
        msvc_internal_data::eh::exception_descriptor const* const exc_dsc = eh_type::get_exception_descriptor(exc_rec);
        void const* const exc_object = eh_type::get_exception_object(exc_rec);

        if ( exc_object && exc_dsc )
        {
          for ( ; try_cur != try_end; try_cur.next() )
          {
            if ( try_cur->low_level <= state && state <= try_cur->high_level )
            {
              for ( catch_iterator catch_block(*try_cur, dc.ImageBase); catch_block.valid(); catch_block.next() )
              {
                for ( catchable_type_iterator catchable_type(*catchable_table_iterator(exc_dsc, dc.ImageBase), dc.ImageBase); catchable_type.valid(); catchable_type.next() )
                {
                  if
                  (
                    eh_type::match
                    (
                      *exc_dsc
                    , catchable_type
                    , catchable_typeinfo_iterator(*catchable_type, dc.ImageBase)
                    , catch_typeinfo_iterator(*catch_block, dc.ImageBase)
                    , catch_block->attributes
                    )
                  )
                  {
                    eh_type::copy_exception_object
                    (
                      exc_object
                    , func_frame
                    , catch_block
                    , catchable_type
                    , dc
                    );
                    unwind_stack
                    (
                      exc_rec
                    , func_dsc
                    , func_frame
                    , frame
                    , try_cur->low_level
                    , catch_handler_iterator(*catch_block, dc.ImageBase)
                    , dc
                    );
                  #if defined ( CFG_EH_STACK_WALKER )
                    if ( exc_rec.ExceptionInformation[EXCPTR_FLAGS] & EXCEPTION_FLAG_STACKWALKER_UNWIND )
                    {
                      return;  // delegate the unwind duties back to the stack_walker
                    }
                  #endif
                    goto next_try_block;  // don't come here if unwound properly
                  }
                }
              }
            }
next_try_block : ;
          }
        }
      }
    }
    else  // ...for remaining SEH-exceptions
    {
      if ( STATUS_BREAKPOINT != exc_rec.ExceptionCode )  //// wouldn't touch this code
      {
      //// TODO SEH translator
#if !( defined (NT_KERNEL_MODE) && defined (_M_ARM) )
  // WinRT (at least 6.2.9200) ke-mode RtlUnwindEx()-->RtlRestoreContext() doesn't support stack consolidation mode, so let's skip 
  // the SEH catching by (...) facility
        for ( ; try_cur != try_end; try_cur.next() )
        {
          if ( try_cur->low_level <= state && state <= try_cur->high_level )
          {
            catch_rev_iterator catch_block(*try_cur, dc.ImageBase); 
            if
            (
              catch_block.valid()
            &&
              !catch_typeinfo_iterator(*catch_block, dc.ImageBase).valid()
            )  // check the last catch in the corresponding try is the '(...)'
            {
              unwind_stack
              (
                exc_rec
              , func_dsc
              , func_frame
              , frame
              , try_cur->low_level
              , catch_handler_iterator(*catch_block, dc.ImageBase)
              , dc
              );
            #if defined ( CFG_EH_STACK_WALKER )
              return;  // could anyone achieve here ? hardly... but hell knows... let's just delegate the decision back to the stack_walker
            #endif
            }
          }
        }
#endif
      }
    }

  }


  namespace aux_
  {
    msvc_internal_data::eh::func_descriptor const* const get_function_descriptor(::DISPATCHER_CONTEXT const& dc)
    {
      return reinterpret_cast<msvc_internal_data::eh::func_descriptor const*>(dc.ImageBase + *reinterpret_cast<rva_t*>(dc.HandlerData));
    }
  }


  ::EXCEPTION_DISPOSITION frame_handler3
  (
    ::EXCEPTION_RECORD    const&  exc_rec
  , frame_ptr_t           const&  frame
  , ::CONTEXT             const&  context
  , ::DISPATCHER_CONTEXT  const&  dc
  )
  {
    // the function this handler is invoked on behalf of
    func_descriptor_iterator const func_dsc(aux_::get_function_descriptor(dc));

    //// check if this frame handler is responsible for the incoming EXCEPTION_RECORD
    if ( eh::EXCEPTION_CODE_CPP == exc_rec.ExceptionCode )  // cpp-exception
    {
      if
      (
        ARRAYSIZE_EXCPTR_NOOBJ <= exc_rec.NumberParameters
      && 
        EXCEPTION_OPCODE_NO_EXC_OBJ == exc_rec.ExceptionInformation[EXCPTR_OPCODE]
      )
      {
        // no duties for this frame handler, the catch block handler is responsible for an exception object searching
        return ::ExceptionContinueSearch;
      }
    }
    else if ( STATUS_UNWIND_CONSOLIDATE != exc_rec.ExceptionCode )  // filter out other no cpp-exceptions
    {
      if
      (
        func_dsc->magic_number >= msvc_internal_data::eh::EH_VC8
      &&
        (func_dsc->flags & msvc_internal_data::eh::FLAG_EHs)
      )
      {
        //  compiled with -EHs option - nothing to do with foreign structured exception codes (besides EXCEPTION_CODE_CPP)
        return ::ExceptionContinueSearch;
      }
    }


    //// proceed the unwind and return
    if ( exc_rec.ExceptionFlags & EXCEPTION_UNWIND )
    {
      if ( func_dsc->unwind_array_size )
      {
        if
        (
          (exc_rec.ExceptionFlags & EXCEPTION_TARGET_UNWIND)
        &&
          STATUS_UNWIND_CONSOLIDATE == exc_rec.ExceptionCode
        )
        {
        // unwinding the target frame
          if
          (
            ARRAYSIZE_EXCPTR_UNW == exc_rec.NumberParameters
          &&
            EXCEPTION_OPCODE_STACK_CONSOLIDATE == exc_rec.ExceptionInformation[EXCPTR_OPCODE]
          )
          {
            unwind_frame
            (
              func_dsc
            , reinterpret_cast<funcframe_ptr_t const> (exc_rec.ExceptionInformation[EXCPTR_UNW_FUNCTION_FRAME])
            , static_cast<ehstate_t>                  (exc_rec.ExceptionInformation[EXCPTR_UNW_TARGET_STATE])
            , dc
            );
          }
        }
        else
        {
        // unwinding the nested frames
          funcframe_ptr_t func_frame = eh_state::function_frame(func_dsc, frame, dc);
          ehstate_t target_state = -1;
          ehstate_t const state = eh_state::from_dc(func_dsc, dc);
          for ( try_rev_iterator try_block(*func_dsc, dc.ImageBase); try_block.valid(); try_block.next() )
          {
            if ( state > try_block->high_level && state <= try_block->catch_level )
            {
              target_state = try_block->high_level;
              break;
            }
          }
          unwind_frame(func_dsc, func_frame, target_state, dc);
        }  
      }

      return ::ExceptionContinueSearch;
    }


    //// else find and invoke the handler
    find_matching_catch_block
    (
      exc_rec
    , func_dsc
    , frame
    , dc
    );
    //  does not return if matching catch-block has been found (if the standard NT exception dispatcher is used)
    //  or returns and delegates the unwinding duties to a custom dispatching routine
    return ::ExceptionContinueSearch;
  }


}  // namespace eh_engine


}  // namespace eh
}  // namespace cpprtl


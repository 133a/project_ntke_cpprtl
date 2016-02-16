/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"

#include "eh_msvc_internal_data.h"
#include "eh_msvc_internal_data_aux.h"
#include "eh_exception_registration.h"
#include "eh_engine.h"
#include "eh_engine_defs.h"
#include "eh_stack_walker.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


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

  enum
  {
    INVALID   = -2,
    EMPTY     = -1,
  };


  void try_range
  (
    func_descriptor_iterator  const&  func_dsc
  , int                               catch_depth
  , ehstate_t                 const&  state
  , try_iterator                   &  try_begin
  , try_iterator                   &  try_end
  )
  {
    try_begin.deface();
    try_end.deface();
    
    try_rev_iterator try_dsc(try_end);
    try_iterator tmp(try_end);
    while (catch_depth >= 0)
    {
      try_dsc.next();
      if ( !try_dsc.valid() || (try_dsc->high_level < state && state <= try_dsc->catch_level) )
      {
        --catch_depth;
        try_end = tmp;
        tmp = try_dsc;
      }
    }

    (try_begin = try_dsc).next();
  }


  ehstate_t get_current_state
  (
    eh_engine::exception_registration  const* const  exc_reg
  , func_descriptor_iterator           const&        func_dsc
  )
  {
  //  TODO check -
  //  do not return just the value 'exc_reg->state_id'
  //  because of mscl_x86 sets the 'int state_id' by command such as 'mov byte ptr [ebp-4],5' for functions with 'func_dsc->unwind_array_size <= 0x80', i.e. writes only lower byte of int.
  //  if we have previously handled exception for this frame we can have written 'state_id=-1' while unwinding that in turn would lead in to a junk in the hi bytes of that int
    return func_dsc->unwind_array_size > 0x80 ? exc_reg->state_id : exc_reg->state_id & 0xff;
  }


  void set_current_state
  (
    eh_engine::exception_registration       * const  exc_reg
  , ehstate_t                          const&        state
  )
  {
    exc_reg->state_id = state;
  }

} // namespace eh_state



namespace eh_type
{


  msvc_internal_data::eh::exception_descriptor const* get_exception_descriptor(::EXCEPTION_RECORD  const* const  exc_rec)
  {
    if ( eh_engine::EXCEPTION_OPCODE_THROW == exc_rec->ExceptionInformation[eh_engine::EXCPTR_OPCODE] )
    {
      return reinterpret_cast<msvc_internal_data::eh::exception_descriptor*>(exc_rec->ExceptionInformation[eh_engine::EXCPTR_THR_THROWINFO]);
    }
    return 0;
  }


  void* get_exception_object(::EXCEPTION_RECORD  const* const  exc_rec)
  {
    if ( eh_engine::EXCEPTION_OPCODE_THROW == exc_rec->ExceptionInformation[eh_engine::EXCPTR_OPCODE] )
    {
      return reinterpret_cast<void*>(exc_rec->ExceptionInformation[eh_engine::EXCPTR_THR_THROWOBJECT]);
    }
    return 0;
  }


  bool match
  (
    msvc_internal_data::eh::exception_descriptor  const* const  exc_dsc
  , catchable_type_iterator                       const&        catchable_type
  , catchable_typeinfo_iterator                   const&        catchable_typeinfo
  , catch_typeinfo_iterator                       const&        catch_typeinfo
  , unsigned                                      const&        catch_attr
  )
  {
    if (0 == *catch_typeinfo || 0 == catch_typeinfo->name) // ( ... ) check
    {
      return true;
    }

    if (*catchable_typeinfo != *catch_typeinfo && !aux_::strzcmp(&catchable_typeinfo->name, &catch_typeinfo->name)) // type_info equality check
    {
      return false;
    }

    if 
    (
         ( (catchable_type->attributes & msvc_internal_data::eh::EXCEP_REFERENCE)  &&  !(catch_attr & msvc_internal_data::eh::CATCH_REFERENCE) )
      || ( (exc_dsc->attributes & msvc_internal_data::eh::EXCEP_CONST)             &&  !(catch_attr & msvc_internal_data::eh::CATCH_CONST)     )
      || ( (exc_dsc->attributes & msvc_internal_data::eh::EXCEP_VOLATILE)          &&  !(catch_attr & msvc_internal_data::eh::CATCH_VOLATILE)  )
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
    if (cast_info.vbase_table_offset >= 0)
    {
      ptr += cast_info.vbase_table_offset;
      ptr += *reinterpret_cast<int*>(*reinterpret_cast< ::size_t*>(ptr) + cast_info.vbase_disp_offset);
    }
    ptr += cast_info.subtype_offset;
    return reinterpret_cast<void*>(ptr);
  }



  void copy_exception_object
  (
    eh_engine::exception_registration       *        exc_reg
  , void                               const* const  exc_object
  , catch_iterator                     const&        catch_block
  , catchable_type_iterator            const&        catchable_type
  )
  {
    if (exc_reg && exc_object && catch_block->exc_offset)
    {
      __try
      {
        catch_typeinfo_iterator type_dsc(*catch_block);
        if (type_dsc.valid() && type_dsc->name)
        {
          ::size_t dst_addr = exc_reg->x86_ebp() + catch_block->exc_offset;
  
          if (catch_block->attributes & msvc_internal_data::eh::CATCH_REFERENCE)
          {
            *reinterpret_cast<void**>(dst_addr) = pointer_cast(exc_object, catchable_type->cast_info);
          }
          else if (catchable_type->attributes & msvc_internal_data::eh::EXCEP_SIMPLE_TYPE)
          {
            aux_::memcpy(reinterpret_cast<void*>(dst_addr), exc_object, catchable_type->size);
            if (sizeof(void*) == catchable_type->size)
            {
              *reinterpret_cast<void**>(dst_addr) = pointer_cast(*reinterpret_cast<void**>(dst_addr), catchable_type->cast_info);
            }
          }
          else // UDT
          {
            void* casted_exc_object = pointer_cast(exc_object, catchable_type->cast_info);
            if (!catchable_type->cctor)
            {
              aux_::memcpy(reinterpret_cast<void*>(dst_addr), casted_exc_object, catchable_type->size);
            }
            else
            {
              if (catchable_type->attributes & msvc_internal_data::eh::EXCEP_VIRTUAL_BASE)
              {
                (reinterpret_cast<msvc_internal_data::eh::aux_::obj*>(dst_addr)->**cctorvb_iterator(*catchable_type))(casted_exc_object, 1);
              }
              else
              {
                (reinterpret_cast<msvc_internal_data::eh::aux_::obj*>(dst_addr)->**cctor_iterator(*catchable_type))(casted_exc_object);
              }
            }
          }
        }
      }
      __except (eh::aux_::invalid_exception(GetExceptionCode(), eh::EXCEPTION_SUBCODE_CCTOR_THROW) , EXCEPTION_CONTINUE_SEARCH)
      {
      }
    }
  }



  void destroy_exception_object(::EXCEPTION_RECORD  const* const  exc_rec)
  {
    if (exc_rec  &&  eh::EXCEPTION_CODE_CPP == exc_rec->ExceptionCode  &&  eh_engine::EXCEPTION_OPCODE_THROW == exc_rec->ExceptionInformation[eh_engine::EXCPTR_OPCODE])
    {
      msvc_internal_data::eh::exception_descriptor const* exc_dsc = eh_type::get_exception_descriptor(exc_rec);
      void* exc_object = eh_type::get_exception_object(exc_rec);
      if (exc_object && exc_dsc)
      {
        dtor_iterator dtor(exc_dsc);
        if ( dtor.valid() )
        {
          __try
          {
            (reinterpret_cast<msvc_internal_data::eh::aux_::obj*>(exc_object)->**dtor)();
          }
          __except (eh::aux_::invalid_exception(GetExceptionCode(), eh::EXCEPTION_SUBCODE_DTOR_THROW) , EXCEPTION_CONTINUE_SEARCH)
          {
          }
        }
      }
    }
  }

}  //  namespace eh_type


namespace eh_engine
{


  void unwind_frame
  (
    exception_registration         * const  exc_reg
  , func_descriptor_iterator  const&        func_dsc
  , ehstate_t                 const&        unwind_state
  , ehstate_t                 const&        target_state  =  eh_state::EMPTY
  )
  {
    ehstate_t current_state = unwind_state;
    unwind_iterator unwind_entry(*func_dsc);

    while (current_state > target_state  &&  unwind_entry[current_state].valid())
    {
      current_state = unwind_entry->prev_state;
      unwind_action_iterator unwind_action(*unwind_entry);
      if ( unwind_action.valid() )
      {
        eh_state::set_current_state(exc_reg, current_state);
        __try
        {
          msvc_internal_data::eh::aux_::funclet::invoke(*unwind_action, exc_reg);
        }
        __except (eh::aux_::invalid_exception(GetExceptionCode(), eh::EXCEPTION_SUBCODE_UNWIND_THROW) , EXCEPTION_CONTINUE_SEARCH)
        {
        }
      }
    }
    eh_state::set_current_state(exc_reg, current_state);
  }


  namespace catch_aux
  {

    int call_catch_block_rethrow_seh_filter
    (
      ::EXCEPTION_POINTERS  *        xp 
    , ::EXCEPTION_RECORD    * const  cur_exc
    )
    {
      if (eh::EXCEPTION_CODE_CPP == xp->ExceptionRecord->ExceptionCode)
      {
        ::EXCEPTION_RECORD* new_exc = xp->ExceptionRecord;
        if (new_exc)
        {

          if (EXCEPTION_OPCODE_THROW == new_exc->ExceptionInformation[EXCPTR_OPCODE])
          {
            if (0 == new_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT])
            {
              new_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT]      = cur_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT];
              new_exc->ExceptionInformation[EXCPTR_THR_THROWINFO]        = cur_exc->ExceptionInformation[EXCPTR_THR_THROWINFO];
              new_exc->ExceptionInformation[EXCPTR_THR_PREV_EXCEPTION]   = reinterpret_cast< ::ULONG_PTR>(cur_exc);
            // our exception object is propagated to the outer scope so we are no longer responsible for it's destruction.
              cur_exc->ExceptionInformation[EXCPTR_FLAGS]           |= EXCEPTION_FLAG_OBJECT_RETHROWED;
            // delegate the exception object's destruction to the outer scope's catch-block caller finally handler
              new_exc->ExceptionInformation[EXCPTR_FLAGS]           &= ~EXCEPTION_FLAG_OBJECT_RETHROWED;
            }
            else if (cur_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT] == new_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT])
            {
              new_exc->ExceptionInformation[EXCPTR_THR_PREV_EXCEPTION]   = reinterpret_cast< ::ULONG_PTR>(cur_exc);
            // our exception object is propagated to the outer scope so we are no longer responsible for it's destruction.
              cur_exc->ExceptionInformation[EXCPTR_FLAGS]           |= EXCEPTION_FLAG_OBJECT_RETHROWED;
            // delegate the exception object's destruction to the outer scope's catch-block caller finally handler
              new_exc->ExceptionInformation[EXCPTR_FLAGS]           &= ~EXCEPTION_FLAG_OBJECT_RETHROWED;
            }

            return EXCEPTION_CONTINUE_SEARCH;
          }

          if (EXCEPTION_OPCODE_NO_EXC_OBJ == new_exc->ExceptionInformation[EXCPTR_OPCODE])
          {
            ::EXCEPTION_RECORD* const rec_patch = reinterpret_cast< ::EXCEPTION_RECORD*>(new_exc->ExceptionInformation[EXCPTR_NOOBJ_EXCREC_PTR]);
            if (rec_patch && EXCEPTION_OPCODE_THROW == rec_patch->ExceptionInformation[EXCPTR_OPCODE])
            {
              rec_patch->ExceptionInformation[EXCPTR_THR_THROWOBJECT]      = cur_exc->ExceptionInformation[EXCPTR_THR_THROWOBJECT];
              rec_patch->ExceptionInformation[EXCPTR_THR_THROWINFO]        = cur_exc->ExceptionInformation[EXCPTR_THR_THROWINFO];
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


    struct catch_guard_exception_registration
      : public exception_registration
    {
      msvc_internal_data::eh::func_descriptor const*   func_dsc;
      msvc_internal_data::eh::exception_registration*  cur_reg;
      int                                              catch_depth;


      static ::EXCEPTION_DISPOSITION __cdecl cg_handler
      (
        ::EXCEPTION_RECORD                  *  exc_rec
      , catch_guard_exception_registration  *  cg_reg
      , void                                *  context
      , void                                *  dc
      )
      {
        __asm cld
        return eh_engine::frame_handler3(exc_rec, cg_reg->cur_reg, context, dc, cg_reg->func_dsc, cg_reg, cg_reg->catch_depth);
      }
    };

  }  //  namespace catch_aux



  void target_unwind_AND_call_catch_block_AND_continue
  (
    exception_registration         * const  exc_reg
  , ::EXCEPTION_RECORD             * const  cur_exc
  , void                      const* const  context
  , void                      const* const  dc
  , catch_handler_iterator    const&        handler
  , func_descriptor_iterator  const&        func_dsc
  , ehstate_t                 const&        target_unwind_state
  , ehstate_t                 const&        catch_block_level
  , exception_registration         * const  cg_reg
  , int                       const         catch_depth
  )
  {    //   do NOT use the objects with the destructor semantics at this scope because of the 'continuation::invoke()' just cuts the stack up.

  //  ( 1 ) do complete the target unwind
    unwind_frame
    (
      exc_reg
    , func_dsc
    , eh_state::get_current_state(exc_reg, func_dsc)
    , target_unwind_state
    );
    eh_state::set_current_state(exc_reg, catch_block_level);

  //  ( 2 ) call catch block
    msvc_internal_data::eh::continuation_ft cont = 0;
    ::size_t const x86_esp = exc_reg->x86_esp();

    __try
    {
      __try
      {
        catch_aux::catch_guard_exception_registration catch_reg;
        catch_reg.handler     = &catch_aux::catch_guard_exception_registration::cg_handler;
        catch_reg.func_dsc    = *func_dsc;
        catch_reg.cur_reg     = exc_reg;
        catch_reg.catch_depth = catch_depth + 1;
        catch_reg.link();

        cont = msvc_internal_data::eh::aux_::funclet::invoke(*handler, exc_reg);

        catch_reg.unlink();
      }
      __except (catch_aux::call_catch_block_rethrow_seh_filter(GetExceptionInformation(), cur_exc))
      {
      }
    }
    __finally
    {
      exc_reg->x86_esp() = x86_esp;
      if ( cont  &&  !(cur_exc->ExceptionInformation[EXCPTR_FLAGS] & EXCEPTION_FLAG_OBJECT_RETHROWED) )
      {
        eh_type::destroy_exception_object(cur_exc);
      }
    }

  //  ( 3 ) invoke the continuation
    eh_state::set_current_state(exc_reg, eh_state::INVALID);
    if (cont)
    {
      exception_registration::head()->unlink();                           //  unlink the current frame 'exception_registration' 'cuz the continuation just sets the new stack frame and jumps,
      msvc_internal_data::eh::aux_::continuation::invoke(cont, exc_reg);  //  so the current frame cleanup duties are got just never invoked...
    }
  }


  namespace aux_
  {

    void target_unwind_callback(::EXCEPTION_RECORD* unw_exc_rec)
    {
      target_unwind_AND_call_catch_block_AND_continue
      (
        reinterpret_cast< exception_registration* >  ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_EXCEPTION_REGISTRATION] )
      , reinterpret_cast< ::EXCEPTION_RECORD* >      ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_EXCEPTION_RECORD] )
      , reinterpret_cast< void* >                    ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_CONTEXT_RECORD] )
      , reinterpret_cast< void* >                    ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_DISPATCHER_CONTEXT] )
      , catch_handler_iterator                       ( reinterpret_cast<msvc_internal_data::eh::catch_handler_ft> ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_CATCH_HANDLER] ) )
      , func_descriptor_iterator                     ( reinterpret_cast<msvc_internal_data::eh::func_descriptor*>( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_FUNC_DESCRIPTOR] ) )
      , static_cast< ehstate_t >                     ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_TARGET_UNWIND_STATE] )
      , static_cast< ehstate_t >                     ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_CATCH_BLOCK_LEVEL] )
      , reinterpret_cast< exception_registration* >  ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_CATCH_GUARD_REGISTRATION] )
      , static_cast< int >                           ( unw_exc_rec->ExceptionInformation[EXCPTR_TUW_CATCH_DEPTH] )
      );
    }

  }


#ifdef _MSC_VER
#  pragma optimize("g", off)
#endif 

// TODO ? using ::RtlUnwind() wrapper
// um msvcrt.dll, km ntoskrnl
// extern "C" __global_unwind2(void*);

  // win sdk/ddk ::RtlUnwind() wrapper
  void unwind_nested_frames
  (
    exception_registration  * const  exc_reg
  , ::EXCEPTION_RECORD      * const  exc_rec
  )
  {
    void* r = 0;
    __asm
    {
      mov  r, offset ret_addr  //  continuation point for RtlUnwind()
      push ebx  //  RtlUnwind() is known to junk these registers
      push esi  //  ^
      push edi  //  ^
    }
    IRQL_CHECK ( <=DISPATCH_LEVEL )  //  RtlUnwind()
    ::RtlUnwind(exc_reg, r, exc_rec, 0);
ret_addr:
    __asm
    {
      pop edi
      pop esi
      pop ebx
    }
    exc_rec->ExceptionFlags &= ~EXCEPTION_UNWINDING;
    return;
  }

#ifdef _MSC_VER
#  pragma optimize("", on)
#endif



  void unwind_stack_and_catch
  (
    exception_registration         * const  exc_reg
  , ::EXCEPTION_RECORD             * const  exc_rec
  , void                      const* const  context
  , void                      const* const  dc
  , catch_handler_iterator    const&        handler
  , try_iterator              const&        try_block
  , func_descriptor_iterator  const&        func_dsc
  , exception_registration         * const  cg_reg
  , int                       const         catch_depth
  )
  {
  // at first let's check whether we are delaing with the 'stack walker'
    if
    (
         eh::EXCEPTION_CODE_CPP == exc_rec->ExceptionCode
      && EXCEPTION_OPCODE_THROW == exc_rec->ExceptionInformation[EXCPTR_OPCODE]
      && ( EXCEPTION_FLAG_STACKWALKER_UNWIND & exc_rec->ExceptionInformation[EXCPTR_FLAGS] )
    )
    {
    //  well, we are being invoked by the 'stack_walk()', so let's delegate the unwinding duties thereto back
      ::EXCEPTION_RECORD& unw_exc_rec = *reinterpret_cast< ::EXCEPTION_RECORD*>(exc_rec->ExceptionInformation[EXCPTR_THR_UNWIND_EXCREC]);

      unw_exc_rec.ExceptionCode     = eh::EXCEPTION_CODE_CPP;
      unw_exc_rec.ExceptionFlags   |= EXCEPTION_NONCONTINUABLE;
      unw_exc_rec.ExceptionRecord   = 0;
      unw_exc_rec.ExceptionAddress  = 0;

      unw_exc_rec.ExceptionInformation[EXCPTR_OPCODE]                        = EXCEPTION_OPCODE_TARGET_UNWIND;
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_CALLBACK_ADDR]             = reinterpret_cast< ::ULONG_PTR>( &aux_::target_unwind_callback );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_EXCEPTION_REGISTRATION]    = reinterpret_cast< ::ULONG_PTR>( exc_reg );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_EXCEPTION_RECORD]          = reinterpret_cast< ::ULONG_PTR>( exc_rec );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_CONTEXT_RECORD]            = reinterpret_cast< ::ULONG_PTR>( context );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_DISPATCHER_CONTEXT]        = reinterpret_cast< ::ULONG_PTR>( dc );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_CATCH_HANDLER]             = reinterpret_cast< ::ULONG_PTR>( *handler );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_TARGET_UNWIND_STATE]       = try_block->low_level ;          
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_CATCH_BLOCK_LEVEL]         = try_block->high_level + 1 ;          
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_FUNC_DESCRIPTOR]           = reinterpret_cast< ::ULONG_PTR>( *func_dsc );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_CATCH_GUARD_REGISTRATION]  = reinterpret_cast< ::ULONG_PTR>( cg_reg );
      unw_exc_rec.ExceptionInformation[EXCPTR_TUW_CATCH_DEPTH]               = catch_depth;

      unw_exc_rec.NumberParameters = ARRAYSIZE_EXCPTR_TUW;
      
      return;  //  into the stack walker
    }

    
  // in the case we are being invoked by some other exception-dispatching code - proceed with unwinding tasks by standard means
    unwind_nested_frames(cg_reg ? cg_reg : exc_reg, exc_rec);
    
    target_unwind_AND_call_catch_block_AND_continue
    (
      exc_reg
    , exc_rec
    , context
    , dc
    , handler
    , func_dsc
    , try_block->low_level
    , try_block->high_level + 1
    , cg_reg
    , catch_depth
    );
  }



  void throw_exception_no_exception_object(::EXCEPTION_RECORD* const rec)
  {
    ::EXCEPTION_RECORD exc_rec  = { 0 };
    exc_rec.ExceptionCode       = eh::EXCEPTION_CODE_CPP;
    exc_rec.ExceptionFlags      = 0;  // this is continuable exception - if catchblock-handler would fill the necessary ptrs it returns ExceptionContinueExecution
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



  void throw_exception(void const* const exc_object, msvc_internal_data::eh::exception_descriptor const* const exc_descr)
  {
    ::EXCEPTION_RECORD exc_rec  = { 0 };
    exc_rec.ExceptionCode       = eh::EXCEPTION_CODE_CPP;
    exc_rec.ExceptionFlags     |= EXCEPTION_NONCONTINUABLE;
    exc_rec.ExceptionRecord     = 0;
    exc_rec.ExceptionAddress    = 0;
    exc_rec.ExceptionInformation[EXCPTR_OPCODE]           = EXCEPTION_OPCODE_THROW;
    exc_rec.ExceptionInformation[EXCPTR_THR_THROWOBJECT]  = reinterpret_cast< ::ULONG_PTR>(exc_object);
    exc_rec.ExceptionInformation[EXCPTR_THR_THROWINFO]    = reinterpret_cast< ::ULONG_PTR>(exc_descr);
    exc_rec.NumberParameters = ARRAYSIZE_EXCPTR_THROW;

    if ( !exc_object || !exc_descr )
    {
      // if we are here the 'throw;' statement occured and we try to get exception info from previous scope's rethrow-seh-filter by continuable seh-exception
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
    ::EXCEPTION_RECORD             *        exc_rec
  , exception_registration         *        exc_reg
  , void                      const* const  context
  , void                      const* const  dc
  , func_descriptor_iterator  const&        func_dsc
  , exception_registration         *        cg_reg
  , int                                     catch_depth
  )
  {
    try_iterator try_cur(*func_dsc);
    try_iterator try_end(*func_dsc);
    ehstate_t state = eh_state::get_current_state(exc_reg, func_dsc);
    eh_state::try_range(func_dsc, catch_depth, state, try_cur, try_end);

    if ( eh::EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )  //// ...for cpp-exception
    {
      msvc_internal_data::eh::exception_descriptor const* exc_dsc = eh_type::get_exception_descriptor(exc_rec);
      void const* exc_object = eh_type::get_exception_object(exc_rec);

      if (exc_object && exc_dsc)
      {
        for ( ; try_cur != try_end; try_cur.next())
        {
          if (try_cur->low_level <= state && state <= try_cur->high_level)
          {
            for (catch_iterator catch_block(*try_cur); catch_block.valid(); catch_block.next())
            {
              for (catchable_type_iterator catchable_type(*catchable_table_iterator(exc_dsc)); catchable_type.valid(); catchable_type.next())
              {
                if
                (
                  eh_type::match
                  (
                    exc_dsc
                  , catchable_type
                  , catchable_typeinfo_iterator(*catchable_type)
                  , catch_typeinfo_iterator(*catch_block)
                  , catch_block->attributes
                  )
                )
                {
                  eh_type::copy_exception_object(exc_reg, exc_object, catch_block, catchable_type);
                  unwind_stack_and_catch
                  (
                    exc_reg
                  , exc_rec
                  , context 
                  , dc
                  , catch_handler_iterator(*catch_block)
                  , try_cur
                  , func_dsc
                  , cg_reg
                  , catch_depth
                  );
                #if defined ( CFG_EH_STACK_WALKER )
                  if ( exc_rec->ExceptionInformation[EXCPTR_FLAGS] & EXCEPTION_FLAG_STACKWALKER_UNWIND )
                  {
                    return;  //  delegate the unwind duties back to the stack_walker
                  }
                #endif
                  goto next_try_block;   // don't come here if properly unwound
                }
              }
            }
          }
next_try_block : ;
        }
      }
    }
    else  //// ...for no-cpp exception - trying to find 'catch(...)' statement (-EH (for older mscl) or -EHa compiler options)
    {
      if ( STATUS_BREAKPOINT != exc_rec->ExceptionCode )  //// wouldn't touch this code
      {
      //// TODO SEH translator
        for ( ; try_cur != try_end; try_cur.next())
        {
          if (try_cur->low_level <= state && state <= try_cur->high_level)
          {
            catch_rev_iterator catch_block(*try_cur); 
            if ( catch_block.valid()  &&  !catch_typeinfo_iterator(*catch_block).valid() )  // check the last catch in appropriate try is the '...'
            {
              unwind_stack_and_catch
              (
                exc_reg
              , exc_rec
              , context
              , dc
              , catch_handler_iterator(*catch_block)
              , try_cur
              , func_dsc
              , cg_reg
              , catch_depth
              );
            #if defined ( CFG_EH_STACK_WALKER )
              return;  // could anyone achieve here ? hardly... but hell knows... let's just delegate the decision to the stack_walker
            #endif
            }
          }
        }
      }
    }
  }



  ::EXCEPTION_DISPOSITION frame_handler3
  (
    ::EXCEPTION_RECORD                            *        exc_rec
  , exception_registration                        *        exc_reg
  , void                                     const* const  context
  , void                                     const* const  dc
  , msvc_internal_data::eh::func_descriptor  const* const  func_descr    // taken by entry pt from x86::eax
  , exception_registration                        *        cg_reg        // = 0
  , int                                                    catch_depth   // = 0
  )
  {
    func_descriptor_iterator const func_dsc(func_descr);

    //// check if there are the duties for our frame handler
    if ( eh::EXCEPTION_CODE_CPP != exc_rec->ExceptionCode )  // filter no cpp-exception
    {
      if ( func_dsc->magic_number >= msvc_internal_data::eh::EH_VC8  &&  (func_dsc->flags & msvc_internal_data::eh::FLAG_EHs) )
      {
        //  compiled with -EHs option - nothing to do with foreign structured exception codes (besides EXCEPTION_CODE_CPP)
        return ::ExceptionContinueSearch;
      }
    }
    else // filter cpp-exception
    {
      if (ARRAYSIZE_EXCPTR_NOOBJ <= exc_rec->NumberParameters  &&  EXCEPTION_OPCODE_NO_EXC_OBJ == exc_rec->ExceptionInformation[EXCPTR_OPCODE])
      {
        // no duties for the frame handler in such a case
        return ::ExceptionContinueSearch;
      }
    }

    //// proceed an unwinding task and return
    if (exc_rec->ExceptionFlags & EXCEPTION_UNWIND) // ::RtlUnwind() sets this flag
    {
      if (func_dsc->unwind_array_size  &&  0 == catch_depth) // check if frame unwind is really needed
      {
        unwind_frame(exc_reg, func_dsc, eh_state::get_current_state(exc_reg, func_dsc));
      }
      return ::ExceptionContinueSearch;
    }

    //// or find handler and spawn unwinding and catching...
    find_matching_catch_block
    (
      exc_rec    
    , exc_reg    
    , context    
    , dc         
    , func_dsc   
    , cg_reg     
    , catch_depth
    );
    //  do not return if matching catch-block has been found or
    //  return the filled target-unwind exception record to the stack walker (for CFG_EH_STACK_WALKER)

    return ::ExceptionContinueSearch;
  }
      

}  //  namespace eh_engine


}  //  namespace eh
}  //  namespace cpprtl


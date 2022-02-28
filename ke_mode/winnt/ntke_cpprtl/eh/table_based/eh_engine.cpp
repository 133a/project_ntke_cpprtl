//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_framework.h"

#include "eh_data.h"
#include "eh_data_iterator.h"
#include "eh_data_4.h"
#include "eh_exception_code.h"
#include "eh_engine.h"
#include "eh_context.h"
#include "eh_aux.h"


namespace cpprtl { namespace eh
{
  void stack_walk(::EXCEPTION_RECORD&, nvcontext&, size_t const = 0);  // fwd


  // eh_funclet.<arch>.asm
  extern "C"
  {
    void cpprtl_eh_run_dtor     (msvc_data::object_type* this_ptr, msvc_data::dtor_ft dtor);
    void cpprtl_eh_run_cctor    (msvc_data::object_type* this_ptr, msvc_data::object_type const* that_ptr, msvc_data::cctor_ft cctor);
    void cpprtl_eh_run_cctor_vb (msvc_data::object_type* this_ptr, msvc_data::object_type const* that_ptr, int, msvc_data::cctor_vb_ft cctor_vb);
    void cpprtl_eh_run_unwind_funclet
    (
      msvc_data::object_type* pobj, function_frame_t frame, msvc_data::funclet_ft funclet
    #if defined (_M_ARM) || defined (_M_ARM64)
    , void const* nvctx
    #endif
    );
    size_t cpprtl_eh_run_catch_block
    (
      msvc_data::funclet_ft funclet, function_frame_t frame, ::EXCEPTION_RECORD* exc_rec
    #if defined (_M_ARM) || defined (_M_ARM64)
    , void const* nvctx
    #endif
    );

    // eh_funclet.<arch>.asm cpprtl_run_xtor() frame handlers implementation
    unsigned cpprtl_eh_dtor_handler(::EXCEPTION_RECORD* exc_rec, void*, void*, void*)
    {
      if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_DTOR_THROW);
      }
      return ::ExceptionContinueSearch;
    }
    unsigned cpprtl_eh_cctor_handler(::EXCEPTION_RECORD* exc_rec, void*, void*, void*)
    {
      if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_CCTOR_THROW);
      }
      return ::ExceptionContinueSearch;
    }
    // eh_funclet.<arch>.asm cpprtl_run_fuclet() frame handler implementation
    unsigned cpprtl_eh_unwind_handler(::EXCEPTION_RECORD* exc_rec, void*, void*, void*)
    {
      if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_UNWIND_THROW);
      }
      return ::ExceptionContinueSearch;
    }
  }  // extern "C"


  class exception_object
  {
    msvc_data::exception_object_descriptor const* exc_descriptor;
    size_t                                        exc_object_addr;
    imagebase_t                                   throw_imagebase;

    bool match
    (
      msvc_data::type_descriptor  const* const  catch_typeinfo
    , unsigned                    const         catch_attributes
    , msvc_data::type_descriptor  const* const  catchable_typeinfo
    , unsigned                    const         catchable_attributes
    ) const
    {
      // (...) check
      if ( !catch_typeinfo || !catch_typeinfo->name[0] )
      {
        return true;
      }
      // type_info equality check
      if
      (
        catchable_typeinfo != catch_typeinfo &&
        !aux_::strzeq(catchable_typeinfo->name, catch_typeinfo->name)
      )
      {
        return false;
      }
      return !
      (
        ( (catchable_attributes       & msvc_data::EXCOBJECT_REFERENCE) && !(catch_attributes & msvc_data::CATCH_REFERENCE) ) ||
        ( (exc_descriptor->attributes & msvc_data::EXCOBJECT_CONST)     && !(catch_attributes & msvc_data::CATCH_CONST)     ) ||
        ( (exc_descriptor->attributes & msvc_data::EXCOBJECT_UNALIGNED) && !(catch_attributes & msvc_data::CATCH_UNALIGNED) ) ||
        ( (exc_descriptor->attributes & msvc_data::EXCOBJECT_VOLATILE)  && !(catch_attributes & msvc_data::CATCH_VOLATILE)  )
      );
    }

  public:
    explicit exception_object(::EXCEPTION_RECORD const& exc_rec)
      : exc_descriptor  ( reinterpret_cast<msvc_data::exception_object_descriptor*>(exc_rec.ExceptionInformation[EXCDATA_THROW_INFO]) )
      , exc_object_addr ( exc_rec.ExceptionInformation[EXCDATA_THROW_OBJECT] )
      , throw_imagebase ( exc_rec.ExceptionInformation[EXCDATA_THROW_IMAGEBASE] )
    {}

    msvc_data::catchable_type_descriptor const* match
    (
      msvc_data::type_descriptor  const* const  catch_typeinfo
    , unsigned                    const         catch_attributes
    ) const                  
    {
      for ( msvc_data::catchable_type_iterator catchable_type(*exc_descriptor, throw_imagebase); catchable_type.valid(); catchable_type.next() )
      {
        if ( match(catch_typeinfo, catch_attributes, &*msvc_data::type_descriptor_accessor(*catchable_type, throw_imagebase), catchable_type->attributes) )
        {
          return &*catchable_type;
        }
      }
      return 0;
    }

    template <typename catch_block_descriptor_type>
    void copy_to_catch_object
    (
      catch_block_descriptor_type           const&  catch_block
    , function_frame_t                      const&  function_frame
    , msvc_data::catchable_type_descriptor  const&  catchable_type
    ) const
    {
      if ( catch_block.object_offset )  // catch by name
      {
        size_t catch_object_addr = reinterpret_cast<size_t>(function_frame) + catch_block.object_offset;
        if ( catch_block.attributes & msvc_data::CATCH_REFERENCE )
        {
          *reinterpret_cast<size_t*>(catch_object_addr) = pointer_cast(exc_object_addr, catchable_type.cast_info);
        }
        else if ( catchable_type.attributes & msvc_data::EXCOBJECT_SIMPLE_TYPE )
        {
          aux_::memcpy(reinterpret_cast<void*>(catch_object_addr), reinterpret_cast<void*>(exc_object_addr), catchable_type.size);
          if ( sizeof(void*) == catchable_type.size )
          {
            *reinterpret_cast<size_t*>(catch_object_addr) = pointer_cast(*reinterpret_cast<size_t const*>(catch_object_addr), catchable_type.cast_info);
          }
        }
        else  // UDT
        {
          size_t const exc_subobject_addr = pointer_cast(exc_object_addr, catchable_type.cast_info);
          if ( !catchable_type.cctor )  // POD if empty
          {
            aux_::memcpy(reinterpret_cast<void*>(catch_object_addr), reinterpret_cast<void const*>(exc_subobject_addr), catchable_type.size);
          }
          else
          {
            if ( catchable_type.attributes & msvc_data::EXCOBJECT_HAS_VIRTUAL_BASE )
            {
              cpprtl_eh_run_cctor_vb
              (
                reinterpret_cast<msvc_data::object_type*>(catch_object_addr)
              , reinterpret_cast<msvc_data::object_type*>(exc_subobject_addr)
              , 1
              , *msvc_data::cctor_vb_accessor(catchable_type, throw_imagebase)
              );
            }
            else
            {
              cpprtl_eh_run_cctor
              (
                reinterpret_cast<msvc_data::object_type*>(catch_object_addr)
              , reinterpret_cast<msvc_data::object_type*>(exc_subobject_addr)
              , *msvc_data::cctor_accessor(catchable_type, throw_imagebase)
              );
            }
          }
        }
      }
    }

    void destroy() const
    {
      msvc_data::dtor_accessor dtor(*exc_descriptor, throw_imagebase);
      if ( *dtor )  // may be empty
      {
        cpprtl_eh_run_dtor(reinterpret_cast<msvc_data::object_type*>(exc_object_addr), *dtor);
      }
    }
  };


  // eh_funclet.<arch>.asm cpprtl_eh_run_catch_block() frame handler payload
  extern "C"
  unsigned cpprtl_eh_catch_block_handler
  (
    ::EXCEPTION_RECORD         * const  new_exc
  , ::EXCEPTION_RECORD         * const  cur_exc
  , void                  const* const
  , ::DISPATCHER_CONTEXT       * const  dispatcher
  )
  {
    if ( EXCEPTION_CODE_CPP == new_exc->ExceptionCode )
    {
      if ( !(new_exc->ExceptionFlags & EXCEPTION_UNWIND) )  // passthrough the exception object when dispatching
      {
        if ( new_exc->ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_NO_EXCEPTION_OBJECT )
        {
          new_exc->ExceptionInformation[EXCDATA_THROW_OBJECT]    = cur_exc->ExceptionInformation[EXCDATA_THROW_OBJECT];
          new_exc->ExceptionInformation[EXCDATA_THROW_INFO]      = cur_exc->ExceptionInformation[EXCDATA_THROW_INFO];
          new_exc->ExceptionInformation[EXCDATA_THROW_IMAGEBASE] = cur_exc->ExceptionInformation[EXCDATA_THROW_IMAGEBASE];
        // put the flag that our scope (and may be outer ones) is responsible of the exception object's lifetime
          new_exc->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_RETHROWN;
          new_exc->ExceptionInformation[EXCDATA_FLAGS] &= ~EXCEPTION_FLAG_NO_EXCEPTION_OBJECT;
          return ExceptionDispatchCPP;
        }
        if (cur_exc->ExceptionInformation[EXCDATA_THROW_OBJECT] == new_exc->ExceptionInformation[EXCDATA_THROW_OBJECT])
        {
        // our exception object is propagated to the next scope so we are no longer responsible for it's destruction.
          cur_exc->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_RETHROWN;
        // delegate the exception object's destruction to the next scope's catch-block caller
          new_exc->ExceptionInformation[EXCDATA_FLAGS] &= ~EXCEPTION_FLAG_OBJECT_RETHROWN;
        }
        if ( cur_exc->ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_FH4 )
        {
          new_exc->ExceptionInformation[EXCDATA_FH4_TRANSIENT_STATE] = cur_exc->ExceptionInformation[EXCDATA_FH4_CATCH_LEVEL];
          new_exc->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_FH4_TRANSIENT_STATE;
        }
      }
      else  // unwind, destroy the exception object
      {
        if ( !(cur_exc->ExceptionInformation[EXCDATA_FLAGS] & (EXCEPTION_FLAG_OBJECT_RETHROWN | EXCEPTION_FLAG_OBJECT_DESTRUCTED)) )
        {
          exception_object(*cur_exc).destroy();
          cur_exc->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_DESTRUCTED;
        }
        if ( cur_exc->ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_FH4 )
        {
          new_exc->ExceptionInformation[EXCDATA_FH4_TRANSIENT_STATE] = cur_exc->ExceptionInformation[EXCDATA_TARGET_STATE];
          new_exc->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_FH4_TRANSIENT_STATE;
        }
      }
      dispatcher->EstablisherFrame = cur_exc->ExceptionInformation[EXCDATA_TARGET_FRAME];  // catch-block nesting scope
      return ExceptionCollisionCPP;
    }
    return ::ExceptionContinueSearch;
  }


  rva_t ip2rva(::DISPATCHER_CONTEXT const& dispatcher)
  {
    rva_t ip_rva = static_cast<rva_t>(dispatcher.ControlPc - dispatcher.ImageBase);  // NOTE narrowing
  #if defined (_M_ARM)
    if ( dispatcher.ControlPcIsUnwound )
    {
      ip_rva -= 2;
    }
  #elif defined (_M_ARM64)
    if ( dispatcher.ControlPcIsUnwound )
    {
      ip_rva -= 4;
    }
  #endif
    return ip_rva;
  }
}}  // namespace cpprtl::eh


namespace cpprtl { namespace eh { namespace fh3
{
  class function_frame_data
  {
    struct unwind_data_t
    {
      msvc_data::frame_state_t state;
      msvc_data::frame_state_t max_state;

      static unwind_data_t* get(msvc_data::function_descriptor const& function, size_t const function_frame_addr)
      {
        return reinterpret_cast<unwind_data_t*>(function_frame_addr + function.unwind_data_offset);
      }
    };

    msvc_data::try_block_descriptor const* in_catch;
    size_t                   function_frame_addr;
    unwind_data_t*           unwind_data;
    msvc_data::frame_state_t state_ip;

    static msvc_data::frame_state_t ip2state(msvc_data::function_descriptor const& function, ::DISPATCHER_CONTEXT const& dispatcher)
    {
      rva_t const ip_rva = ip2rva(dispatcher);
      msvc_data::frame_state_t state = msvc_data::frame_state::EMPTY;
      for ( msvc_data::ip2state_iterator ip2state(function, dispatcher.ImageBase); ip2state.valid(); ip2state.next() )
      {
        if ( ip_rva < ip2state->ip_rva )
        {
          break;
        }
        state = ip2state->state;
      }
      return state;
    }

    void unwind_data_update() const
    {
      if ( unwind_data->max_state < state_ip )
      {
        unwind_data->state = unwind_data->max_state = state_ip;
      }
    }

  public:
    function_frame_data  // intermediate frames ctor
    (
      funclet_frame_t                 const   funclet_frame
    , msvc_data::function_descriptor  const&  function
    , ::DISPATCHER_CONTEXT            const&  dispatcher
    )
      : in_catch            ( 0 )
      , function_frame_addr ( reinterpret_cast<size_t>(funclet_frame) )
      , unwind_data         ( 0 )
      , state_ip            ( ip2state(function, dispatcher) )
    {
      msvc_data::try_block_iterator try_block(function, dispatcher.ImageBase);
      for ( try_block.last(); try_block.valid() && !in_catch; try_block.prev() )
      {
        if ( (state_ip > try_block->high_level) && (state_ip <= try_block->catch_level) )  // the state is in one of catch blocks of this try{}
        {
          for
          (
            msvc_data::catch_block_iterator catch_block(*try_block, dispatcher.ImageBase)
          ; catch_block.valid()
          ; catch_block.next()
          )
          {
            if ( catch_block->funclet == dispatcher.FunctionEntry->BeginAddress )
            {
             // the catch block is found, obtain the hosting function frame
            #if defined (_M_AMD64) || defined (_M_X64)
              function_frame_addr = *reinterpret_cast<size_t const*>(function_frame_addr + catch_block->frame_offset);
            #elif defined (_M_ARM) || defined (_M_ARM64)
              function_frame_addr = *reinterpret_cast<size_t const*>(function_frame_addr);  // saved at cpprtl_eh_run_catch_block frame top
            #else
            #  error check $(target.arch)
            #endif
              in_catch = &*try_block;  // cache the try block by the way
              break;
            }
          }
        }
      }
      unwind_data = unwind_data_t::get(function, function_frame_addr);
      unwind_data_update();
    }

    function_frame_data  // the target frame ctor
    (
      function_frame_t                const   function_frame
    , msvc_data::function_descriptor  const&  function
    , ::DISPATCHER_CONTEXT            const&  dispatcher
    )
      : in_catch            ( 0 )
      , function_frame_addr ( reinterpret_cast<size_t>(function_frame) )
      , unwind_data         ( unwind_data_t::get(function, function_frame_addr) )
      , state_ip            ( ip2state(function, dispatcher) )
    {
      unwind_data_update();
    }

    bool is_catch_funclet() const  // NOTE irrelevant for target frames
    {
      return 0 != in_catch;
    }

    // frame raw value
    operator function_frame_t() const { return reinterpret_cast<function_frame_t>(function_frame_addr); }

    // target state for intermediate stack frames
    msvc_data::frame_state_t intermediate_target_state() const
    {
      if ( in_catch )
      {
        return in_catch->high_level;
      }
      return msvc_data::frame_state::EMPTY;
    }
    // frame-stored unwind state access
    msvc_data::frame_state_t current_state() const
    {
      if ( msvc_data::frame_state::INVALID == unwind_data->state )
      {
        return state_ip;
      }
      return unwind_data->state;
    }
    void current_state(msvc_data::frame_state_t st) const
    {
      unwind_data->state = st;
    }
    static void unwind_data_reset(msvc_data::function_descriptor const& function, function_frame_t const function_frame)
    {
      unwind_data_t* const unwind_data = unwind_data_t::get(function, reinterpret_cast<size_t>(function_frame));
      unwind_data->state     = msvc_data::frame_state::INVALID;
      unwind_data->max_state = msvc_data::frame_state::EMPTY;
    }

    void try_range
    (
      msvc_data::function_descriptor  const&  function
    , ::DISPATCHER_CONTEXT            const&  dispatcher
    , msvc_data::try_block_iterator        &  try_begin
    , msvc_data::try_block_iterator        &  try_end
    ) const
    {
      try_begin.deface();
      try_end.deface();
      msvc_data::frame_state_t const state = current_state();

      for ( msvc_data::try_block_iterator try_block(function, dispatcher.ImageBase); try_block.valid(); try_block.next() )
      {
        if
        (
          in_catch  &&
          ( (try_block->low_level <= in_catch->high_level) || (try_block->high_level > in_catch->catch_level) )
        )
        {
          continue;
        }
        if ( (state >= try_block->low_level) && (state <= try_block->high_level) )
        {
          if ( !try_begin.valid() )
          {
            try_begin = try_block;
          }
          try_end = try_block;
        }
      } 
      try_end.next();
    }
  };


  size_t run_catch_block(::EXCEPTION_RECORD& exc_rec)
  {
    size_t const address = cpprtl_eh_run_catch_block
    (
      reinterpret_cast<msvc_data::funclet_ft> (exc_rec.ExceptionInformation[EXCDATA_FH3_CATCH_FUNCLET])
    , reinterpret_cast<function_frame_t>      (exc_rec.ExceptionInformation[EXCDATA_FUNCTION_FRAME])
    , &exc_rec
    #if defined (_M_ARM) || defined (_M_ARM64)
    , reinterpret_cast<void const*>           (exc_rec.ExceptionInformation[EXCDATA_NVCONTEXT])
    #endif
    );

    if ( !(exc_rec.ExceptionInformation[EXCDATA_FLAGS] & (EXCEPTION_FLAG_OBJECT_RETHROWN | EXCEPTION_FLAG_OBJECT_DESTRUCTED)) )
    {
      exception_object(exc_rec).destroy();
      exc_rec.ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_DESTRUCTED;
    }

    function_frame_data::unwind_data_reset
    (
      *reinterpret_cast<msvc_data::function_descriptor const*>(exc_rec.ExceptionInformation[EXCDATA_FH3_FUNCTION_DESCRIPTOR])
    , reinterpret_cast<function_frame_t>(exc_rec.ExceptionInformation[EXCDATA_FUNCTION_FRAME])
    );

    return address;
  }


  void unwind_frame
  (
    msvc_data::function_descriptor  const&  function
  , function_frame_data             const&  frame_data
  , msvc_data::frame_state_t        const   target_state
  , ::DISPATCHER_CONTEXT            const&  dispatcher
  )
  {
    msvc_data::frame_state_t current_state = frame_data.current_state();
    msvc_data::unwind_iterator unwind(function, dispatcher.ImageBase);
    while ( current_state > target_state )
    {
      if ( !unwind.valid(current_state) )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NO_UNWIND_DATA);
      }
      msvc_data::funclet_accessor unwind_funclet(unwind[current_state], dispatcher.ImageBase);
      frame_data.current_state(current_state = unwind->prev_state);
      if ( *unwind_funclet )  // may be empty
      {
        cpprtl_eh_run_unwind_funclet
        (
          0, frame_data, *unwind_funclet
        #if defined (_M_ARM) || defined (_M_ARM64)
        , dispatcher.NonVolatileRegisters
        #endif
        );
      }
    }
  }


  unsigned cpp_frame_handler  // fh3
  (
    ::EXCEPTION_RECORD                   &  exc_rec
  , msvc_data::function_descriptor  const&  function
  , funclet_frame_t                 const   funclet_frame
  , ::DISPATCHER_CONTEXT            const&  dispatcher
  )
  {
    // check if there are some duties for our frame handler
    if ( EXCEPTION_CODE_CPP != exc_rec.ExceptionCode )  // filter out no cpp-exceptions, ignore -EHa
    {
      return ::ExceptionContinueSearch;
    }
    else if ( exc_rec.ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_NO_EXCEPTION_OBJECT )  // filter out cpp-exceptions
    {
      // this handler is only responsible for catch-block matching and unwinding
      return ::ExceptionContinueSearch;
    }

    // proceed the unwinding task and return...
    if ( exc_rec.ExceptionFlags & EXCEPTION_UNWIND )
    {
      if ( exc_rec.ExceptionFlags & EXCEPTION_TARGET_UNWIND )
      {
        // the target frame, unwind the try block
        if ( function.unwind_array_size )
        {
          function_frame_data const frame_data(reinterpret_cast<function_frame_t>(exc_rec.ExceptionInformation[EXCDATA_FUNCTION_FRAME]), function, dispatcher);
          msvc_data::frame_state_t const target_state = static_cast<msvc_data::frame_state_t>(exc_rec.ExceptionInformation[EXCDATA_TARGET_STATE]);
          unwind_frame(function, frame_data, target_state, dispatcher);
        }
        return ExceptionDispatchCPP;
      }
      else
      {
        // an intermediate stack frame
        if ( function.unwind_array_size )
        {
          function_frame_data const frame_data(funclet_frame, function, dispatcher);
          msvc_data::frame_state_t const target_state = frame_data.intermediate_target_state();
          unwind_frame(function, frame_data, target_state, dispatcher);
        }  
      }
      return ::ExceptionContinueSearch;
    }

    // ...or find the matching catch-block and init the stack unwind
    {
      exception_object exc_object(exc_rec);

      function_frame_data function_frame(funclet_frame, function, dispatcher);
      msvc_data::frame_state_t const state = function_frame.current_state();

      msvc_data::try_block_iterator try_block(function, dispatcher.ImageBase);
      msvc_data::try_block_iterator try_end = try_block;
      function_frame.try_range(function, dispatcher, try_block, try_end);

      for ( ; try_block != try_end; try_block.next() )
      {
        if ( try_block->low_level <= state && state <= try_block->high_level )
        {
          for ( msvc_data::catch_block_iterator catch_block(*try_block, dispatcher.ImageBase); catch_block.valid(); catch_block.next() )
          {
            if
            (
              msvc_data::catchable_type_descriptor const* const catchable_type =
                exc_object.match(&*msvc_data::type_descriptor_accessor(*catch_block, dispatcher.ImageBase), catch_block->attributes)
            )
            {
              exc_object.copy_to_catch_object(*catch_block, function_frame, *catchable_type);
              // the catch block responsible for the exception have been found
              exc_rec.ExceptionInformation[EXCDATA_CATCH_IMAGEBASE]          = dispatcher.ImageBase;
              exc_rec.ExceptionInformation[EXCDATA_FUNCTION_FRAME]           = reinterpret_cast< ::ULONG_PTR>(function_frame_t(function_frame));
              exc_rec.ExceptionInformation[EXCDATA_TARGET_FRAME]             = reinterpret_cast< ::ULONG_PTR>(funclet_frame);
              exc_rec.ExceptionInformation[EXCDATA_TARGET_STATE]             = try_block->low_level;
              exc_rec.ExceptionInformation[EXCDATA_RUN_CATCH_BLOCK]          = reinterpret_cast< ::ULONG_PTR>(run_catch_block);
              exc_rec.ExceptionInformation[EXCDATA_FH3_FUNCTION_DESCRIPTOR]  = reinterpret_cast< ::ULONG_PTR>(&function);
              exc_rec.ExceptionInformation[EXCDATA_FH3_CATCH_FUNCLET]        = reinterpret_cast< ::ULONG_PTR>(*msvc_data::funclet_accessor(*catch_block, dispatcher.ImageBase));
              exc_rec.NumberParameters = ARRAYSIZE_EXCDATA_FH3;
              // pass the unwind duties back through the stack_walk()
              return ExceptionDispatchCPP;
            }
          }
        }
      }
      // no matching catch-block found, check the function isn't 'noexcept'-specified
      if ( !function_frame.is_catch_funclet() )
      {
        if ( (function.signature >= msvc_data::MARK_VC8) && (function.flags & msvc_data::FUNCTION_NOEXCEPT) )
        {
          aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NOEXCEPT_THROW);
        }
      }
    }
    return ::ExceptionContinueSearch;
  }

}}}  // namespace cpprtl::eh::fh3


namespace cpprtl { namespace eh { namespace fh4
{
  size_t run_catch_block(::EXCEPTION_RECORD& exc_rec)
  {
    msvc_data::fh4::catch_block_descriptor const catch_block
    (
      reinterpret_cast<msvc_data::fh4::catch_block_data_packed> (exc_rec.ExceptionInformation[EXCDATA_FH4_CATCH_BLOCK_DATA])
    , static_cast<rva_t>                                        (exc_rec.ExceptionInformation[EXCDATA_FH4_FUNCLET_ENTRY])
    , static_cast<imagebase_t>                                  (exc_rec.ExceptionInformation[EXCDATA_CATCH_IMAGEBASE])
    );

    size_t address = cpprtl_eh_run_catch_block
    (
      catch_block.catch_funclet
    , reinterpret_cast<function_frame_t> (exc_rec.ExceptionInformation[EXCDATA_FUNCTION_FRAME])
    , &exc_rec
    #if defined (_M_ARM) || defined (_M_ARM64)
    , reinterpret_cast<void const*>      (exc_rec.ExceptionInformation[EXCDATA_NVCONTEXT])
    #endif
    );

    exc_rec.ExceptionInformation[EXCDATA_FLAGS] &= ~EXCEPTION_FLAG_FH4_TRANSIENT_STATE;

    if ( address < msvc_data::fh4::catch_block_descriptor::CONTINUATION_ARRAY_LEN )
    {
      if ( !(address < catch_block.continuation_count()) )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NO_CONTINUATION_ADDRESS);
      }
      address = catch_block.continuation[address];
    }

    if ( !(exc_rec.ExceptionInformation[EXCDATA_FLAGS] & (EXCEPTION_FLAG_OBJECT_RETHROWN | EXCEPTION_FLAG_OBJECT_DESTRUCTED)) )
    {
      exception_object(exc_rec).destroy();
      exc_rec.ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_DESTRUCTED;
    }
    return address;
  }


  namespace { enum { FRAME_STATE_SHIFT = 2 }; }

  void unwind_frame
  (
    msvc_data::fh4::function_descriptor  const&  function
  , ::EXCEPTION_RECORD                        &  exc_rec
  , ::DISPATCHER_CONTEXT                 const&  dispatcher
  , msvc_data::frame_state_t                     current_state
  , msvc_data::frame_state_t             const   target_state
  )
  {
    msvc_data::fh4::unwind_iterator unwind(function, function_frame_t(function), function.image_base);
    unwind.state(current_state);
    while ( current_state > target_state )
    {
      if ( !unwind.valid() )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NO_UNWIND_DATA);
      }
      msvc_data::funclet_ft const unwind_funclet = unwind->funclet;
      msvc_data::object_type* const pobj = unwind->object;
      unwind.prev();
      current_state = unwind.state();
    // TODO is it necessary to keep track of all unwound states? anyway the collided unwind is prevented by cpprtl_eh_unwind_handler()
      exc_rec.ExceptionInformation[EXCDATA_FH4_UNWIND_STATE] = current_state + FRAME_STATE_SHIFT;
      if ( unwind_funclet )  // may be empty
      {
        cpprtl_eh_run_unwind_funclet
        (
          pobj, function, unwind_funclet
        #if defined (_M_ARM) || defined (_M_ARM64)
        , dispatcher.NonVolatileRegisters
        #endif
        );
      }
    }
  }


  unsigned cpp_frame_handler  // fh4
  (
    ::EXCEPTION_RECORD                         &  exc_rec
  , msvc_data::fh4::function_data_packed  const   function_data
  , funclet_frame_t                       const   funclet_frame
  , ::DISPATCHER_CONTEXT                  const&  dispatcher
  )
  {
    // check if there are some duties for our frame handler
    if ( EXCEPTION_CODE_CPP != exc_rec.ExceptionCode )  // filter out no cpp-exceptions, ignore -EHa
    {
      return ::ExceptionContinueSearch;
    }
    else if ( exc_rec.ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_NO_EXCEPTION_OBJECT )  // filter out cpp-exceptions
    {
      // this handler is only responsible for catch-block matching and unwinding
      return ::ExceptionContinueSearch;
    }
    msvc_data::fh4::function_descriptor const function(function_data, funclet_frame, dispatcher.FunctionEntry->BeginAddress, dispatcher.ImageBase);
    msvc_data::frame_state_t state = msvc_data::frame_state::EMPTY;
    if ( exc_rec.ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_FH4_TRANSIENT_STATE )
    {
      state = msvc_data::frame_state_t(exc_rec.ExceptionInformation[EXCDATA_FH4_TRANSIENT_STATE]);
      exc_rec.ExceptionInformation[EXCDATA_FLAGS] &= ~EXCEPTION_FLAG_FH4_TRANSIENT_STATE;
    }
    else
    {
      state = function.ip2state(ip2rva(dispatcher));
    }

    // proceed the unwinding task and return...
    if ( exc_rec.ExceptionFlags & EXCEPTION_UNWIND )
    {
      exc_rec.ExceptionInformation[EXCDATA_FH4_UNWIND_STATE] = state + FRAME_STATE_SHIFT;
      if ( exc_rec.ExceptionFlags & EXCEPTION_TARGET_UNWIND )
      {
        // the target frame, unwind the try block
        if ( function.has_unwind_table() )
        {
          msvc_data::frame_state_t const target_state = static_cast<msvc_data::frame_state_t>(exc_rec.ExceptionInformation[EXCDATA_TARGET_STATE]);
          unwind_frame(function, exc_rec, dispatcher, state, target_state);
        }
        return ExceptionDispatchCPP;
      }
      else
      {
        // an intermediate stack frame
        if ( function.has_unwind_table() )
        {
          unwind_frame(function, exc_rec, dispatcher, state, msvc_data::frame_state::EMPTY);
        }
      }
      return ::ExceptionContinueSearch;
    }

    // ...or find the matching catch-block and init the stack unwind
    {
      exception_object exc_object(exc_rec);
      // TODO try_range()
      for ( msvc_data::fh4::try_block_iterator try_block(function, function.image_base); try_block.valid(); try_block.next() )
      {
        if ( try_block->low_level <= state && state <= try_block->high_level )
        {
          for
          (
            msvc_data::fh4::catch_block_iterator catch_block(try_block->catch_block_table, function.entry, function.image_base)
          ; catch_block.valid()
          ; catch_block.next()
          )
          {
            if ( msvc_data::catchable_type_descriptor const* const catchable_type = exc_object.match(catch_block->type_info, catch_block->attributes) )
            {
              exc_object.copy_to_catch_object(catch_block.descriptor(), function, *catchable_type);
              // the catch block responsible for the exception have been found
              exc_rec.ExceptionInformation[EXCDATA_CATCH_IMAGEBASE]       = function.image_base;
              exc_rec.ExceptionInformation[EXCDATA_FUNCTION_FRAME]        = reinterpret_cast< ::ULONG_PTR>(function_frame_t(function));
              exc_rec.ExceptionInformation[EXCDATA_TARGET_FRAME]          = reinterpret_cast< ::ULONG_PTR>(funclet_frame);
              exc_rec.ExceptionInformation[EXCDATA_TARGET_STATE]          = try_block->low_level;
              exc_rec.ExceptionInformation[EXCDATA_RUN_CATCH_BLOCK]       = reinterpret_cast< ::ULONG_PTR>(run_catch_block);
              exc_rec.ExceptionInformation[EXCDATA_FH4_CATCH_BLOCK_DATA]  = reinterpret_cast< ::ULONG_PTR>(catch_block.data());
              exc_rec.ExceptionInformation[EXCDATA_FH4_FUNCLET_ENTRY]     = function.entry;
              exc_rec.ExceptionInformation[EXCDATA_FH4_CATCH_LEVEL]       = try_block->catch_level;
              exc_rec.ExceptionInformation[EXCDATA_FLAGS]                |= EXCEPTION_FLAG_FH4;
              exc_rec.NumberParameters = ARRAYSIZE_EXCDATA_FH4;
              // pass the unwind duties back through the stack_walk()
              return ExceptionDispatchCPP;
            }
          }
        }
      }
      // no matching catch-block found, check the function isn't 'noexcept'-specified
      if ( !function.is_catch_funclet() && function.is_noexcept() )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NOEXCEPT_THROW);
      }
    }
    return ::ExceptionContinueSearch;
  }
}}}  // namespace cpprtl::eh::fh4


//=======================================================
// msvc internally pre-declared EH routines' entry points
//-------------------------------------------------------
namespace cpprtl { namespace eh
{
  extern "C"
  void _CxxThrowException(void* exc_object, ::_ThrowInfo const* exc_dsc)
  {
    nvcontext ctx;
    ::EXCEPTION_RECORD exc_rec;
    for ( size_t idx = 0; idx < EXCEPTION_MAXIMUM_PARAMETERS; ++idx )
    {
      exc_rec.ExceptionInformation[idx] = 0;
    }
    exc_rec.ExceptionCode    = EXCEPTION_CODE_CPP;
    exc_rec.ExceptionFlags   = EXCEPTION_NONCONTINUABLE;
    exc_rec.ExceptionRecord  = 0;
    exc_rec.ExceptionAddress = 0;
    exc_rec.NumberParameters = ARRAYSIZE_EXCDATA_EH;
    exc_rec.ExceptionInformation[EXCDATA_THROW_OBJECT]  = reinterpret_cast< ::ULONG_PTR>(exc_object);
    exc_rec.ExceptionInformation[EXCDATA_THROW_INFO]    = reinterpret_cast< ::ULONG_PTR>(exc_dsc);
    if ( exc_dsc )
    {
      IRQL_CHECK ( <=DISPATCH_LEVEL )  // RtlPcToFileHeader()
      exc_rec.ExceptionInformation[EXCDATA_THROW_IMAGEBASE] = reinterpret_cast< ::ULONG_PTR>
      (
        ::RtlPcToFileHeader
        (
          reinterpret_cast<void*>  (exc_rec.ExceptionInformation[EXCDATA_THROW_INFO])
        , reinterpret_cast<void**> (&exc_rec.ExceptionInformation[EXCDATA_THROW_IMAGEBASE])
        )
      );
    }
    else
    {
      // 'throw;' statement occured and we have to try to get exception info from the previous scope's catch-block handler
      exc_rec.ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_NO_EXCEPTION_OBJECT;
      stack_walk(exc_rec, ctx);
    }
    // dispatching
    stack_walk(exc_rec, ctx);
    if ( !exc_rec.ExceptionInformation[EXCDATA_TARGET_FRAME] )
    {
      aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_ENGINE_DISPATCHING_ERROR);
    }

    // unwinding
    stack_walk(exc_rec, ctx, exc_rec.ExceptionInformation[EXCDATA_TARGET_FRAME]);

    // run catch block 
  #if defined (_M_ARM) || defined (_M_ARM64)
    exc_rec.ExceptionInformation[EXCDATA_NVCONTEXT] = reinterpret_cast< ::ULONG_PTR>(ctx.NV());
  #endif
    ctx.IP() = reinterpret_cast<size_t(*)(::EXCEPTION_RECORD&)>(exc_rec.ExceptionInformation[EXCDATA_RUN_CATCH_BLOCK])(exc_rec);

    // continuation
    ctx.start();

    // never returns
    aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_ENGINE_CONTINUATION_ERROR);
  }
}}  // namespace cpprtl::eh


extern "C"
unsigned __CxxFrameHandler3(::EXCEPTION_RECORD* exc_rec, void* frame, ::CONTEXT*, ::DISPATCHER_CONTEXT* dispatcher)
{
  using namespace cpprtl; using namespace cpprtl::eh;
  return eh::fh3::cpp_frame_handler
  (
    *exc_rec
  , *reinterpret_cast<msvc_data::function_descriptor const*>(dispatcher->ImageBase + reinterpret_cast<rva_t*>(dispatcher->HandlerData)[0])
  , reinterpret_cast<funclet_frame_t>(frame)
  , *dispatcher
  );
}

extern "C"
unsigned __CxxFrameHandler4(::EXCEPTION_RECORD* exc_rec, void* frame, ::CONTEXT*, ::DISPATCHER_CONTEXT* dispatcher)
{
  using namespace cpprtl; using namespace cpprtl::eh;
  return fh4::cpp_frame_handler
  (
    *exc_rec
  , reinterpret_cast<msvc_data::fh4::function_data_packed>(dispatcher->ImageBase + reinterpret_cast<rva_t*>(dispatcher->HandlerData)[0])
  , reinterpret_cast<funclet_frame_t>(frame)
  , *dispatcher
  );
}

//==========================================================
// msvc internally pre-declared GS_EH routines' entry points
//----------------------------------------------------------
#if (_MSC_FULL_VER >= 140050320)  // >=msvc2005
namespace cpprtl { struct gs_handler_data; }

// ntoskrnl.lib for NT_KERNEL_MODE
extern "C" void __GSHandlerCheckCommon(void* frame, ::DISPATCHER_CONTEXT* dispatcher, cpprtl::gs_handler_data const* data);

extern "C"
unsigned __GSHandlerCheck_EH(::EXCEPTION_RECORD* exc_rec, void* frame, ::CONTEXT* context, ::DISPATCHER_CONTEXT* dispatcher)
{
  using namespace cpprtl;
  __GSHandlerCheckCommon(frame, dispatcher, reinterpret_cast<gs_handler_data const*>(&reinterpret_cast<rva_t*>(dispatcher->HandlerData)[1]));
  return __CxxFrameHandler3(exc_rec, frame, context, dispatcher);
}

extern "C"
unsigned __GSHandlerCheck_EH4(::EXCEPTION_RECORD* exc_rec, void* frame, ::CONTEXT* context, ::DISPATCHER_CONTEXT* dispatcher)
{
  using namespace cpprtl;
  __GSHandlerCheckCommon(frame, dispatcher, reinterpret_cast<gs_handler_data const*>(&reinterpret_cast<rva_t*>(dispatcher->HandlerData)[1]));
  return __CxxFrameHandler4(exc_rec, frame, context, dispatcher);
}
#endif  // _MSC_FULL_VER

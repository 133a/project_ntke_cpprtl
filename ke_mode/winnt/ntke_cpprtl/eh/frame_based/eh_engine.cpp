//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_framework.h"

#include "eh_data.h"
#include "eh_data_iterator.h"
#include "eh_cpp_reg.h"
#include "eh_exception_code.h"
#include "eh_engine.h"
#include "eh_aux.h"


namespace cpprtl { namespace eh
{

  void stack_walk(::EXCEPTION_RECORD&, msvc_data::seh::registration* const = 0);  // fwd

  unsigned cpp_frame_handler  // fwd
  (
    ::EXCEPTION_RECORD                   &
  , msvc_data::cpp_registration          *
  , msvc_data::function_descriptor  const&
  , msvc_data::seh::registration         *
  , int                                 
  );


  extern "C"
  {
    // eh_safeseh.x86.asm SafeSEH entry points and impl
    unsigned __cdecl safeseh_dtor_handler(::EXCEPTION_RECORD*, msvc_data::seh::registration*, void*, void*);
    unsigned __cdecl cpprtl_eh_dtor_terminator(::EXCEPTION_RECORD* exc_rec, msvc_data::seh::registration*, void*, void*)
    {
      if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_DTOR_THROW);
      }
      return ::ExceptionContinueSearch;
    }

    unsigned __cdecl safeseh_cctor_handler(::EXCEPTION_RECORD*, msvc_data::seh::registration*, void*, void*);
    unsigned __cdecl cpprtl_eh_cctor_terminator(::EXCEPTION_RECORD* exc_rec, msvc_data::seh::registration*, void*, void*)
    {
      if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_CCTOR_THROW);
      }
      return ::ExceptionContinueSearch;
    }

    unsigned __cdecl safeseh_funclet_handler(::EXCEPTION_RECORD*, msvc_data::seh::registration*, void*, void*);
    unsigned __cdecl cpprtl_eh_funclet_terminator(::EXCEPTION_RECORD* exc_rec, msvc_data::seh::registration*, void*, void*)
    {
      if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_UNWIND_THROW);
      }
      return ::ExceptionContinueSearch;
    }

    unsigned __cdecl safeseh_catch_block_handler(::EXCEPTION_RECORD*, msvc_data::seh::registration* seh_reg, void*, void*);
  }  // extern "C"


  class exception_object
  {
    msvc_data::exception_object_descriptor const* exc_descriptor;
    size_t                                        exc_object_addr;

  public:
    explicit exception_object(::EXCEPTION_RECORD const& exc_rec)
      : exc_descriptor  ( reinterpret_cast<msvc_data::exception_object_descriptor*>(exc_rec.ExceptionInformation[EXCDATA_THROW_INFO]) )
      , exc_object_addr ( exc_rec.ExceptionInformation[EXCDATA_THROW_OBJECT] )
    {}

    void destroy() const
    {
      if ( exc_descriptor->dtor )  // may be empty
      {
        msvc_data::seh::registration dtor_reg;
        dtor_reg.frame_handler = safeseh_dtor_handler;
        msvc_data::seh::link(&dtor_reg);
        (reinterpret_cast<msvc_data::object_type*>(exc_object_addr)->*exc_descriptor->dtor)();
        msvc_data::seh::unlink(&dtor_reg);
      }
    }

    bool match
    (
      msvc_data::catch_block_descriptor  const&  catch_block
    , size_t                             const   frame_base
    ) const
    {
      for ( msvc_data::catchable_type_iterator catchable_type(*exc_descriptor); catchable_type.valid(); catchable_type.next() )
      {
        if ( match(catch_block, *catchable_type) )
        {
          copy_to_catch_object(catch_block, *catchable_type, frame_base);
          return true;
        }
      }
      return false;
    }

  private:
    bool match
    (
      msvc_data::catch_block_descriptor     const&  catch_block
    , msvc_data::catchable_type_descriptor  const&  catchable_type
    ) const
    {
      // (...) check
      if ( !catch_block.type_info || !catch_block.type_info->name[0] )
      {
        return true;
      }
      // type_info equality check
      if
      (
        catchable_type.type_info != catch_block.type_info &&
        !aux_::strzeq(catchable_type.type_info->name, catch_block.type_info->name)
      )
      {
        return false;
      }
      return !
      (
        ( (catchable_type.attributes  & msvc_data::EXCOBJECT_REFERENCE) && !(catch_block.attributes & msvc_data::CATCH_REFERENCE) ) ||
        ( (exc_descriptor->attributes & msvc_data::EXCOBJECT_CONST)     && !(catch_block.attributes & msvc_data::CATCH_CONST)     ) ||
        ( (exc_descriptor->attributes & msvc_data::EXCOBJECT_VOLATILE)  && !(catch_block.attributes & msvc_data::CATCH_VOLATILE)  )
      );
    }

    void copy_to_catch_object
    (
      msvc_data::catch_block_descriptor     const&  catch_block
    , msvc_data::catchable_type_descriptor  const&  catchable_type
    , size_t                                const   frame_base
    ) const
    {
      if ( catch_block.object_offset )  // catch by name
      {
        size_t catch_object_addr = frame_base + catch_block.object_offset;
        if ( catch_block.attributes & msvc_data::CATCH_REFERENCE )
        {
          *reinterpret_cast<size_t*>(catch_object_addr) = pointer_cast(exc_object_addr, catchable_type.cast_info);
        }
        else if ( catchable_type.attributes & msvc_data::EXCOBJECT_SIMPLE_TYPE )
        {
          aux_::memcpy(reinterpret_cast<void*>(catch_object_addr), reinterpret_cast<void const*>(exc_object_addr), catchable_type.size);
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
            msvc_data::seh::registration cctor_reg;
            cctor_reg.frame_handler = safeseh_cctor_handler;
            msvc_data::seh::link(&cctor_reg);
            if ( catchable_type.attributes & msvc_data::EXCOBJECT_HAS_VIRTUAL_BASE )
            {
              (reinterpret_cast<msvc_data::object_type*>(catch_object_addr)->*catchable_type.cctor_vb)
              (reinterpret_cast<msvc_data::object_type*>(exc_subobject_addr), 1);
            }
            else
            {
              (reinterpret_cast<msvc_data::object_type*>(catch_object_addr)->*catchable_type.cctor)
              (reinterpret_cast<msvc_data::object_type*>(exc_subobject_addr));
            }
            msvc_data::seh::unlink(&cctor_reg);
          }
        }
      }
    }
  };


  void unwind_frame
  (
    msvc_data::cpp_registration          &  cpp_reg
  , msvc_data::function_descriptor  const&  function
  , msvc_data::frame_state_t                current_state
  , msvc_data::frame_state_t        const   target_state
  )
  {
    msvc_data::unwind_iterator unwind(function);
    while ( current_state > target_state )
    {
      if ( !unwind.valid(current_state) )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NO_UNWIND_DATA);
      }
      current_state = unwind[current_state].prev_state;
      cpp_reg.frame_state(current_state);
      if ( unwind->funclet )  // may be empty
      {
        msvc_data::seh::registration unwind_reg;
        unwind_reg.frame_handler = safeseh_funclet_handler;
        msvc_data::seh::link(&unwind_reg);
        msvc_data::funclet::run(unwind->funclet, cpp_reg.frame_base());
        msvc_data::seh::unlink(&unwind_reg);
      }
    }
  }


  struct catch_block_registration : msvc_data::seh::registration
  {
    ::EXCEPTION_RECORD                  * exc_rec;
    msvc_data::cpp_registration         * cpp_reg;
    msvc_data::function_descriptor const* function;
    int                                   catch_depth;

    catch_block_registration() { frame_handler = safeseh_catch_block_handler; }
  };


  extern "C"
  unsigned __cdecl cpprtl_eh_catch_block_handler(::EXCEPTION_RECORD* exc_rec, msvc_data::seh::registration* seh_reg, void*, void*)
  {
    catch_block_registration* const catch_reg = static_cast<catch_block_registration*>(seh_reg);
    if ( EXCEPTION_CODE_CPP == exc_rec->ExceptionCode )
    {
      if ( !(exc_rec->ExceptionFlags & EXCEPTION_UNWIND) )  // passthrough the exception object when dispatching
      {
        if ( exc_rec->ExceptionInformation[EXCDATA_FLAGS] & EXCEPTION_FLAG_NO_EXCEPTION_OBJECT )
        {
          exc_rec->ExceptionInformation[EXCDATA_THROW_OBJECT] = catch_reg->exc_rec->ExceptionInformation[EXCDATA_THROW_OBJECT];
          exc_rec->ExceptionInformation[EXCDATA_THROW_INFO]   = catch_reg->exc_rec->ExceptionInformation[EXCDATA_THROW_INFO];
        // put the flag that our scope (or may be some next one) is currently responsible of the exception object's lifetime
          exc_rec->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_RETHROWN;
          exc_rec->ExceptionInformation[EXCDATA_FLAGS] &= ~EXCEPTION_FLAG_NO_EXCEPTION_OBJECT;
          return ExceptionDispatchCPP;
        }
        if (catch_reg->exc_rec->ExceptionInformation[EXCDATA_THROW_OBJECT] == exc_rec->ExceptionInformation[EXCDATA_THROW_OBJECT])
        {
        // our exception object is propagated to the next scope so we are no longer responsible for it's destruction.
          catch_reg->exc_rec->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_RETHROWN;
        // delegate the exception object's destruction to the next scope's catch-block caller
          exc_rec->ExceptionInformation[EXCDATA_FLAGS]            &= ~EXCEPTION_FLAG_OBJECT_RETHROWN;
        }
      }
    }
    unsigned disposition = cpp_frame_handler(*exc_rec, catch_reg->cpp_reg, *(catch_reg->function), catch_reg, catch_reg->catch_depth);
    if ( (EXCEPTION_CODE_CPP == exc_rec->ExceptionCode) && (exc_rec->ExceptionFlags & EXCEPTION_UNWIND) )
    {
    // destroy the exception object while unwinding if needed
      if ( !(catch_reg->exc_rec->ExceptionInformation[EXCDATA_FLAGS] & (EXCEPTION_FLAG_OBJECT_RETHROWN | EXCEPTION_FLAG_OBJECT_DESTRUCTED)) )
      {
        exception_object(*(catch_reg->exc_rec)).destroy();
        catch_reg->exc_rec->ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_DESTRUCTED;
      }
    }
    return disposition;
  }


  __declspec(noreturn)
  void run_catch_block_and_continue(::EXCEPTION_RECORD& exc_rec)
  {
    msvc_data::cpp_registration* const cpp_reg =
      reinterpret_cast<msvc_data::cpp_registration*>(exc_rec.ExceptionInformation[EXCDATA_UW_CPP_REGISTRATION]);
    msvc_data::function_descriptor const& function = 
      *reinterpret_cast<msvc_data::function_descriptor const*>(exc_rec.ExceptionInformation[EXCDATA_UW_FUNCTION_DESCRIPTOR]);
    msvc_data::catch_block_descriptor const& catch_block = 
      *reinterpret_cast<msvc_data::catch_block_descriptor const*>(exc_rec.ExceptionInformation[EXCDATA_UW_CATCH_BLOCK_DESCRIPTOR]);
    int const catch_depth = static_cast<int>(exc_rec.ExceptionInformation[EXCDATA_UW_CATCH_DEPTH]);

    // ( 1 ) call the catch-block
    catch_block_registration catch_reg;
    catch_reg.exc_rec     = &exc_rec;
    catch_reg.cpp_reg     = cpp_reg;
    catch_reg.function    = &function;
    catch_reg.catch_depth = catch_depth + 1;

    // store the current funclet stack, since the catch funclet to be run saves the stack it's being run with into cpp_reg location to support nested try blocks
    size_t const stack_pointer = cpp_reg->stack_pointer();
    size_t const frame_base    = cpp_reg->frame_base();
    msvc_data::seh::link(&catch_reg);
    msvc_data::continuation_ft const continuation = msvc_data::funclet::run(catch_block.funclet, frame_base);
    msvc_data::seh::unlink(&catch_reg);
    cpp_reg->stack_pointer() = stack_pointer;  // restore the current funclet stack

    // ( 2 ) destroy the exception object
    if ( !(exc_rec.ExceptionInformation[EXCDATA_FLAGS] & (EXCEPTION_FLAG_OBJECT_RETHROWN | EXCEPTION_FLAG_OBJECT_DESTRUCTED)) )
    {
      exception_object(exc_rec).destroy();
      exc_rec.ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_OBJECT_DESTRUCTED;
    }
    cpp_reg->frame_state(msvc_data::frame_state::INVALID);

    // ( 3 ) start the continuation
    msvc_data::continuation::start(continuation, frame_base, stack_pointer);
  }


  void try_block_range
  (
    msvc_data::frame_state_t       const   state
  , int                                    catch_depth
  , msvc_data::try_block_iterator       &  begin
  , msvc_data::try_block_iterator       &  end
  )
  {
    begin.deface();
    end.deface();
    
    msvc_data::try_block_iterator try_block = end;
    while ( catch_depth >= 0 )
    {
      try_block.prev();
      if
      (
        !try_block.valid() ||
        (try_block->high_level < state && state <= try_block->catch_level)
      )
      {
        --catch_depth;
        end = begin;
        begin = try_block;
      }
    }
    (begin = try_block).next();
  }


  unsigned cpp_frame_handler
  (
    ::EXCEPTION_RECORD                   &        exc_rec
  , msvc_data::cpp_registration          * const  cpp_reg
  , msvc_data::function_descriptor  const&        function
  , msvc_data::seh::registration         * const  catch_seh_reg = 0
  , int                             const         catch_depth   = 0
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
        msvc_data::try_block_descriptor const& try_block =
          *reinterpret_cast<msvc_data::try_block_descriptor const*>(exc_rec.ExceptionInformation[EXCDATA_UW_TRY_BLOCK_DESCRIPTOR]);
        if ( function.unwind_array_size )
        {
          unwind_frame(*cpp_reg, function, cpp_reg->frame_state(function), try_block.low_level);
        }
        cpp_reg->frame_state(try_block.high_level + 1);  // prepare to run catch block
        return ExceptionDispatchCPP;
      }
      else
      {
        // an intermediate stack frame
        if ( function.unwind_array_size )
        {
          msvc_data::frame_state_t const current_state = cpp_reg->frame_state(function);
          msvc_data::frame_state_t target_state = msvc_data::frame_state::EMPTY;
          if ( catch_depth )  // catch block unwind
          {
            for ( msvc_data::try_block_iterator try_block(function); try_block.valid(); try_block.next() )
            {
             if ( try_block->high_level < current_state && current_state <= try_block->catch_level )
             {
               target_state = msvc_data::unwind_iterator(function)[try_block->high_level + 1].prev_state;  // try_block->high_level
               break;
             }
            }
          }
          unwind_frame(*cpp_reg, function, current_state, target_state);
        }
      }
      return ::ExceptionContinueSearch;
    }

    // ...or find the matching catch-block and init the stack unwind
    {
      exception_object exc_object(exc_rec);

      msvc_data::frame_state_t const state = cpp_reg->frame_state(function);
      msvc_data::try_block_iterator try_block(function);
      msvc_data::try_block_iterator try_block_end(function);
      try_block_range(state, catch_depth, try_block, try_block_end);

      for ( ; try_block != try_block_end; try_block.next() )
      {
        if ( try_block->low_level <= state && state <= try_block->high_level )
        {
          for ( msvc_data::catch_block_iterator catch_block(*try_block); catch_block.valid(); catch_block.next() )
          {
            if ( exc_object.match(*catch_block, cpp_reg->frame_base()) )
            {
              // the catch block responsible for the exception have been found
              exc_rec.ExceptionInformation[EXCDATA_UW_CPP_REGISTRATION]         = reinterpret_cast< ::ULONG_PTR>( cpp_reg );
              exc_rec.ExceptionInformation[EXCDATA_UW_FUNCTION_DESCRIPTOR]      = reinterpret_cast< ::ULONG_PTR>( &function );
              exc_rec.ExceptionInformation[EXCDATA_UW_TRY_BLOCK_DESCRIPTOR]     = reinterpret_cast< ::ULONG_PTR>( &*try_block );
              exc_rec.ExceptionInformation[EXCDATA_UW_CATCH_BLOCK_DESCRIPTOR]   = reinterpret_cast< ::ULONG_PTR>( &*catch_block );
              exc_rec.ExceptionInformation[EXCDATA_UW_CATCH_DEPTH]              = catch_depth;
              exc_rec.ExceptionInformation[EXCDATA_UW_TARGET_SEH_REGISTRATION]  = reinterpret_cast< ::ULONG_PTR>( catch_seh_reg ? catch_seh_reg : cpp_reg );
              // pass the unwind duties back through the stack_walk()
              return ExceptionDispatchCPP;
            }
          }
        }
      }
      // no matching catch-block found, check the function isn't 'noexcept'-specified
      if ( 0 == catch_depth )  // do not terminate when handling a thrown catch block
      {
        if ( (function.signature >= msvc_data::MARK_VC8) && (function.flags & msvc_data::FUNCTION_NOEXCEPT) )
        {
          aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NOEXCEPT_THROW);
        }
      }
    }

    return ::ExceptionContinueSearch;
  }

}}  // namespace cpprtl::eh


//=======================================================
// msvc internally pre-declared EH routines' entry points
//-------------------------------------------------------

extern "C"
void __stdcall _CxxThrowException(void* exc_object, _ThrowInfo const* exc_dsc)
{
  using namespace cpprtl::eh;

  ::EXCEPTION_RECORD exc_rec;
  for ( size_t idx = 0; idx < EXCEPTION_MAXIMUM_PARAMETERS; ++idx )
  {
    exc_rec.ExceptionInformation[idx] = 0;
  }
  exc_rec.ExceptionCode       = EXCEPTION_CODE_CPP;
  exc_rec.ExceptionFlags      = EXCEPTION_NONCONTINUABLE;
  exc_rec.ExceptionRecord     = 0;
  exc_rec.ExceptionAddress    = 0;
  exc_rec.NumberParameters    = ARRAYSIZE_EXCDATA_EH;
  exc_rec.ExceptionInformation[EXCDATA_FLAGS]         = 0;
  exc_rec.ExceptionInformation[EXCDATA_THROW_OBJECT]  = reinterpret_cast< ::ULONG_PTR>(exc_object);
  exc_rec.ExceptionInformation[EXCDATA_THROW_INFO]    = reinterpret_cast< ::ULONG_PTR>(exc_dsc);

  if ( !exc_dsc )
  {
    // 'throw;' statement occured and we have to try to get exception info from the previous scope's catch-block handler
    exc_rec.ExceptionInformation[EXCDATA_FLAGS] |= EXCEPTION_FLAG_NO_EXCEPTION_OBJECT;
    stack_walk(exc_rec);
  }

  // dispatching
  stack_walk(exc_rec);
  if ( !exc_rec.ExceptionInformation[EXCDATA_UW_TARGET_SEH_REGISTRATION] )
  {
    aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_ENGINE_DISPATCHING_ERROR);
  }

  // unwinding
  stack_walk(exc_rec, reinterpret_cast<msvc_data::seh::registration*>(exc_rec.ExceptionInformation[EXCDATA_UW_TARGET_SEH_REGISTRATION]));

  // continuation
  run_catch_block_and_continue(exc_rec);

  // never returns
  aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_ENGINE_CONTINUATION_ERROR);
}

extern "C"
unsigned __cdecl __CxxFrameHandler3(::EXCEPTION_RECORD* exc_rec, cpprtl::eh::msvc_data::seh::registration* seh_reg, void*, void*)
{
  using namespace cpprtl::eh;
  msvc_data::function_descriptor const* function;
#ifdef _M_IX86
  __asm  mov function, eax  // function_descriptor = x86.eax
#else
#  error check $(target.arch)
#endif
  return cpp_frame_handler(*exc_rec, static_cast<msvc_data::cpp_registration*>(seh_reg), *function);
}

extern "C" __declspec(naked)
unsigned __cdecl __CxxFrameHandler(void*, void*, void*, void*)
{
#ifdef _M_IX86
  __asm  jmp __CxxFrameHandler3
#else
#  error check $(target.arch)
#endif
}

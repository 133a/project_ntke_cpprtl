/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"

#include "eh_stack_walker.h"
#include "eh_exception_registration.h"
#include "eh_engine_defs.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


#ifdef _MSC_VER
#  pragma optimize("g", off)
#endif 


namespace cpprtl
{
namespace eh
{
namespace eh_engine
{

  using msvc_internal_data::eh::exception_registration;

////////////////////////////////////////////////////////
////////  EH stack dispatching/unwinding low-level routines
////////////////////////////////////////////////////////

  namespace aux_
  {

    struct dispatcher_context
    {
      exception_registration* exc_reg;  //  keep it at [0] position for the compatibility with os's dispatching/unwinding engines
    };


    struct execute_frame_handler_exception_registration
      : public exception_registration
    {
      enum
      {
        FRAME_HANDLER_EXECUTED_AT_DISPATCHING = 0
      , FRAME_HANDLER_EXECUTED_AT_UNWINDING   = 1 << 0
      };


      int                    flags;
      dispatcher_context*    dsp_ctx;


      static ::EXCEPTION_DISPOSITION __cdecl efh_handler
      (
        ::EXCEPTION_RECORD                            *  exc_rec
      , execute_frame_handler_exception_registration  *  efh_reg
      , ::CONTEXT                                     *  ctx
      , dispatcher_context                            *  dsp_ctx
      )
      {
        if ( efh_reg->flags & FRAME_HANDLER_EXECUTED_AT_UNWINDING )
        {
          if ( exc_rec->ExceptionFlags & EXCEPTION_UNWIND )
          {
            dsp_ctx->exc_reg = efh_reg->dsp_ctx->exc_reg;
            return ExceptionCollidedUnwind;
          }
        }
        else
        {
          if ( ! (exc_rec->ExceptionFlags & EXCEPTION_UNWIND) )
          {
            dsp_ctx->exc_reg = efh_reg->dsp_ctx->exc_reg;
            return ExceptionNestedException;
          }
        }
        return ExceptionContinueSearch;
      }
    };


    int execute_frame_handler
    (
      ::EXCEPTION_RECORD      *  exc_rec
    , exception_registration  *  exc_reg
    , ::CONTEXT               *  ctx
    , dispatcher_context      *  dsp_ctx
    , ::PEXCEPTION_ROUTINE       frame_handler
    )
    {
    //  create and link the custom exception registration
      execute_frame_handler_exception_registration efh_reg;
      efh_reg.flags = execute_frame_handler_exception_registration::FRAME_HANDLER_EXECUTED_AT_DISPATCHING;
      if ( exc_rec->ExceptionFlags & EXCEPTION_UNWIND )
      {
        efh_reg.flags |= execute_frame_handler_exception_registration::FRAME_HANDLER_EXECUTED_AT_UNWINDING;
      }
      efh_reg.dsp_ctx = dsp_ctx;
      efh_reg.handler = &execute_frame_handler_exception_registration::efh_handler;
      efh_reg.link();

    //  invoke the frame handler
      int disposition = frame_handler(exc_rec, exc_reg, ctx, dsp_ctx);

    //  unlink the custom exception registration
      efh_reg.unlink();
    // return the frame disposition
      return disposition;
    }


    enum
    {
      STACK_MISALIGNMENT = (1 << 2) - 1
    };


    bool stack_walk_impl
    (
      ::EXCEPTION_RECORD  &  original_exc_rec
    , ::EXCEPTION_RECORD  &  target_unwind_exc_rec
    )
    {
      ::EXCEPTION_RECORD*  current_exc_rec = &original_exc_rec;
      ::CONTEXT* current_ctx = 0;  //  the stack walker can't be invoked from a trap but just can be invoked as a function call so do we need to maintain the context ?

      exception_registration* prev_exc_reg    = 0;
      exception_registration* nested_exc_reg  = 0;
      exception_registration* target_exc_reg  = 0;
      exception_registration* current_exc_reg = 0; 
      bool unwind_phase = false;

      if 
      (
         eh::EXCEPTION_CODE_CPP == original_exc_rec.ExceptionCode
      && EXCEPTION_OPCODE_THROW == original_exc_rec.ExceptionInformation[EXCPTR_OPCODE]
      )
      {
        target_unwind_exc_rec.ExceptionCode                              =  eh::EXCEPTION_CODE_CPP;
        target_unwind_exc_rec.ExceptionInformation[EXCPTR_OPCODE]        =  EXCEPTION_OPCODE_STACKWALKER_UNWIND;
        original_exc_rec.ExceptionInformation[EXCPTR_THR_UNWIND_EXCREC]  =  reinterpret_cast< ::ULONG_PTR>(&target_unwind_exc_rec);  //  here the frame handler is to save the unwinding info if any
        original_exc_rec.ExceptionInformation[EXCPTR_FLAGS]             |=  EXCEPTION_FLAG_STACKWALKER_UNWIND;
      }

stack_walker_frame_traversing_loop:
      for ( current_exc_reg = exception_registration::head() ; exception_registration::REG_END != current_exc_reg ; )
      {

        if  //  check the frame exception registration consistency
        (
          current_exc_reg > current_exc_reg->next                  //  check the next frame exception registration is at the upper stack position or just the 'END_REG' 
          || ( reinterpret_cast< ::size_t>(current_exc_reg) & STACK_MISALIGNMENT )  //  and is being properly aligned
        )
        {
          goto stack_walker_bad_stack;
        }

        if ( unwind_phase )
        {
          if ( current_exc_reg > target_exc_reg )  //  in such a case the target seems to be just unreachable
          {
            goto stack_walker_bad_stack;
          }
          if ( current_exc_reg == target_exc_reg )  //  the target frame has been reached and we are to perform the target unwind and the continuation
          {
            original_exc_rec.ExceptionFlags  &=  ~EXCEPTION_UNWIND;  //  remove any unwinding flags
            goto stack_walker_target_unwind;
          }
        }

      //  preparing the environment to the frame handler execution
        aux_::dispatcher_context dsp_ctx;
        dsp_ctx.exc_reg  = current_exc_reg;

      //  do invoke the frame handler
        int disposition = aux_::execute_frame_handler
        (
          current_exc_rec
        , current_exc_reg
        , current_ctx
        , &dsp_ctx
        , reinterpret_cast<PEXCEPTION_ROUTINE>(current_exc_reg->handler)
        );

      //  adjust the environment after the frame handler returned
        if ( current_exc_reg == nested_exc_reg )
        {
          current_exc_rec->ExceptionFlags &= ( ~EXCEPTION_NESTED_CALL );
          nested_exc_reg = 0;
        }

      //  check the frame disposition
        switch ( disposition )
        {
        case ExceptionContinueExecution:   //  expected at the dispatching phase
          if ( current_exc_rec->ExceptionFlags & EXCEPTION_NONCONTINUABLE )
          {
            goto stack_walker_noncontinuable;
          }
          goto stack_walker_continue_execution;
          break;

        case ExceptionNestedException:   //  expected at the dispatching phase, 'efh_handler' is expected to fill our dsp_ctx with the values the previous dispatching loop stucked with
          current_exc_rec->ExceptionFlags |= EXCEPTION_NESTED_CALL;
          if ( dsp_ctx.exc_reg > nested_exc_reg )
          {
            nested_exc_reg = dsp_ctx.exc_reg;
          }
          break;

        case ExceptionCollidedUnwind:   //  expected at the unwinding phase, 'efh_handler' is expected to fill our dsp_ctx with the values the previous dispatching loop stucked with
          current_exc_reg = dsp_ctx.exc_reg;  //  let's just preempt the exception registration the previous unwinding loop got stucked with
          break;

        case ExceptionContinueSearch:   //  expected both at the dispatching and unwinding
          if ( !unwind_phase )  //  here to check if the stack unwind should be started or even proceed to the stack consolidation
          {
            if    //  the following flags are expected to be set only at the eh_engine::stack_unwind():
            ( 
                eh::EXCEPTION_CODE_CPP == target_unwind_exc_rec.ExceptionCode
            &&  EXCEPTION_OPCODE_TARGET_UNWIND == target_unwind_exc_rec.ExceptionInformation[EXCPTR_OPCODE] 
            )
            {
            // make the preparations for the unwind phase:
    
              target_exc_reg = reinterpret_cast<exception_registration*>(target_unwind_exc_rec.ExceptionInformation[EXCPTR_TUW_EXCEPTION_REGISTRATION]);
              if ( target_unwind_exc_rec.ExceptionInformation[EXCPTR_TUW_CATCH_GUARD_REGISTRATION] )
              {
                target_exc_reg = reinterpret_cast<exception_registration*>(target_unwind_exc_rec.ExceptionInformation[EXCPTR_TUW_CATCH_GUARD_REGISTRATION]);
              }
              original_exc_rec.ExceptionFlags |= EXCEPTION_UNWINDING;   //  'EXCEPTION_EXIT_UNWIND' isn't expected 'cos we are having the target frame
    
              nested_exc_reg  = 0;
              prev_exc_reg    = 0;
              unwind_phase    = true;                     //  the unwind phase can be started only by these eh-lib frame handler
              goto stack_walker_frame_traversing_loop;    //  follow to the unwinding phase of the stack walking
            }
          }
          break;

        default:
          goto stack_walker_invalid_disposition;  //  the frame handler is not expected to return these values
          break;
        };

      //  preparing to move to the next frame exception registration
        prev_exc_reg     = current_exc_reg;
        current_exc_reg  = current_exc_reg->next;

        if ( unwind_phase )
        {
          prev_exc_reg->unlink();
        }
      }
      goto stack_walker_unexpected;

stack_walker_target_unwind :
      return true;

stack_walker_continue_execution :
      return false;

stack_walker_noncontinuable:
      target_unwind_exc_rec.ExceptionCode    = STATUS_NONCONTINUABLE_EXCEPTION;
      target_unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      target_unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      target_unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(target_unwind_exc_rec);

stack_walker_invalid_disposition:
      target_unwind_exc_rec.ExceptionCode    = STATUS_INVALID_DISPOSITION;
      target_unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      target_unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      target_unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(target_unwind_exc_rec);

stack_walker_bad_stack:
      target_unwind_exc_rec.ExceptionCode    = eh::EXCEPTION_CODE_STACKWALKER_BAD_STACK;
      target_unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      target_unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      target_unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(target_unwind_exc_rec);

stack_walker_unexpected :
      target_unwind_exc_rec.ExceptionCode    = eh::EXCEPTION_CODE_STACKWALKER_UNEXPECTED_RETURN;
      target_unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      target_unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      target_unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(target_unwind_exc_rec);

      return false;  //  could anyone achieve here ?
    }

  }  //  namespace aux_



  void stack_walk(::EXCEPTION_RECORD& original_exc_rec)
  {
  //  do NOT use the objects with destructor semantics at this scope because if the target unwind is to occure the continuation will just cut the stack up

    void* ret_addr           = 0;
    ::size_t prev_stack_ptr  = 0;
    ::size_t prev_frame_ptr  = 0;
    __asm
    {
      mov  eax , [ebp + 4]
      mov  ret_addr , eax   //  load the return address
    }
    original_exc_rec.ExceptionAddress = ret_addr;

    ::EXCEPTION_RECORD target_unwind_exc_rec = { 0 };

    if ( aux_::stack_walk_impl(original_exc_rec, target_unwind_exc_rec) )
    {
    //  the stack_walk_impl() has got the stack unwound up to the target frame and has filled the 'target_unwind_exc_rec' with the data necessary to perform the target unwind and the continuation
      (reinterpret_cast<void (*)(::EXCEPTION_RECORD*)>(target_unwind_exc_rec.ExceptionInformation[EXCPTR_TUW_CALLBACK_ADDR]))(&target_unwind_exc_rec);
    //  just doesn't return into this scope
    }

    //  otherwise the stack_walk_impl() requests for continuing the stack_walk() caller
    return;
  }

} // namespace eh_engine


}  //  namespace eh
}  //  namespace cpprtl


#ifdef _MSC_VER
#  pragma optimize("", on)
#endif


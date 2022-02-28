//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_framework.h"
#include "eh_exception_code.h"
#include "eh_context.h"
#include "eh_aux.h"


namespace cpprtl { namespace eh
{

namespace
{
  #if defined (_M_AMD64) || defined (_M_X64)
  size_t IP(::DISPATCHER_CONTEXT const& dispatcher)
  {
    return dispatcher.ContextRecord->Rip;
  }
  #endif

  #if defined (_M_ARM) || defined (_M_ARM64)
  size_t IP(::DISPATCHER_CONTEXT const& dispatcher)
  {
    size_t ip = dispatcher.ContextRecord->Pc;
    if ( ip && dispatcher.ControlPcIsUnwound )
    {
    #if defined (_M_ARM)
      ip -= 2;
    #elif defined (_M_ARM64)
      ip -= 4;
    #endif
    }
    return ip;
  }
  #endif

  enum { STACK_MISALIGNMENT = (1 << 3) - 1 };  // x64, arm, arm64
}


void stack_walk
(
  ::EXCEPTION_RECORD  &  exc_rec
, nvcontext           &  nv_ctx
, size_t const           target_frame
)
{
  ::CONTEXT ctx;
  ::DISPATCHER_CONTEXT dispatcher;
  dispatcher.ContextRecord      = &ctx;  // BTW _C_specific_handler()/RtlUnwindEx() requires this
#if defined (_M_ARM) || defined (_M_ARM64)
  dispatcher.ControlPcIsUnwound = FALSE;
#endif
  size_t rewind_to_frame = 0;

  IRQL_CHECK ( <= DISPATCH_LEVEL );  // RtlCaptureContext()
  ::RtlCaptureContext(&ctx);

  while ( dispatcher.ControlPc = IP(dispatcher) )  // the frame-traversing loop
  {
    nv_ctx = ctx;  // save the current nv-context
    dispatcher.ContextRecord = &ctx;
    dispatcher.HistoryTable  = 0;
    dispatcher.ScopeIndex    = 0;
  #if defined (_M_AMD64) || defined (_M_X64)
    dispatcher.TargetIp      = 0;
  #elif defined (_M_ARM) || defined (_M_ARM64)
    dispatcher.TargetPc      = 0;
    dispatcher.ControlPcIsUnwound   = TRUE;
    dispatcher.NonVolatileRegisters = reinterpret_cast< ::UCHAR*>(nv_ctx.NV());
  #endif

    IRQL_CHECK ( <= DISPATCH_LEVEL );  // RtlLookupFunctionEntry()
    dispatcher.FunctionEntry = ::RtlLookupFunctionEntry(dispatcher.ControlPc, &dispatcher.ImageBase, dispatcher.HistoryTable);
    if ( !dispatcher.FunctionEntry )
    {
      aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_FUNCTION_NOT_FOUND);
    }

    IRQL_CHECK ( <=DISPATCH_LEVEL )  // ::RtlVirtualUnwind()
    dispatcher.LanguageHandler = RtlVirtualUnwind
      (
        target_frame ? UNW_FLAG_UHANDLER : UNW_FLAG_EHANDLER
      , dispatcher.ImageBase
      , dispatcher.ControlPc
      , dispatcher.FunctionEntry
      , dispatcher.ContextRecord
      , &dispatcher.HandlerData
      , &dispatcher.EstablisherFrame
      , 0
      );
    // ctx contains the previous function context

    // check the unwound data
    if ( dispatcher.EstablisherFrame & STACK_MISALIGNMENT )
    {
      aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_BAD_STACK);
    }
    if ( rewind_to_frame )
    {
      if ( rewind_to_frame == dispatcher.EstablisherFrame )
      {
        rewind_to_frame = 0;
      }
      else
      {
        continue; 
      }
    }
    if ( target_frame )
    {
      if ( target_frame < dispatcher.EstablisherFrame )  // the descending stack is expected
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_UNREACHABLE_TARGET_FRAME);
      }
      if ( target_frame == dispatcher.EstablisherFrame )
      {
        exc_rec.ExceptionFlags |= EXCEPTION_TARGET_UNWIND;
      }
      exc_rec.ExceptionFlags |= EXCEPTION_UNWINDING;
    }

    if ( !dispatcher.LanguageHandler )
    {
      continue; 
    }

    // dispatch the frame disposition
    // neither the original nor the current CONTEXT are maintained
    switch ( dispatcher.LanguageHandler(&exc_rec, reinterpret_cast<void*>(dispatcher.EstablisherFrame), 0, &dispatcher) )
    {
    case ExceptionDispatchCPP:
      exc_rec.ExceptionFlags &= ~EXCEPTION_UNWIND;  // remove unwinding flags if any
      return;  
      break;

    case ExceptionCollisionCPP:
      rewind_to_frame = dispatcher.EstablisherFrame;  // catch block handler saves the funclet frame to be rewound to
      continue;
      break;

    case ExceptionContinueSearch:
      continue;
      break;

    default:  // ExceptionContinueExecution, ExceptionNestedException, ExceptionCollidedUnwind are not expected
      aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_UNSUPPORTED_DISPOSITION);
      break;
    };
  }  // while(dispatcher.ControlPc)

  aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_UNEXPECTED_RETURN);
}

}}  // namespace cpprtl::eh

/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"
#include <check_ct.h>

#include "eh_stack_walker.h"
#include "eh_engine_defs.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


namespace aux_
{

  __declspec ( align(8) )
  class nv_context         //  be accurate to keep this in accordance with the 'CTX_' offsets at the 'eh_restore_context.arm.asm'
  {
  public:
    enum
    {
      R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC, CPSR, FPSCR
    , ULONG_CONTEXT_LENGTH
    };

    enum
    {
      ULONGLONG_CONTEXT_LENGTH = 32
    };


  private:
    ::ULONG      int_context [ULONG_CONTEXT_LENGTH];
    ::ULONGLONG  fp_context  [ULONGLONG_CONTEXT_LENGTH];


  public:

    nv_context& operator = (nv_context const& rhs)
    {
      for ( int i = 0 ; i < ULONG_CONTEXT_LENGTH ; ++i )
      {
        int_context[i] = rhs.int_context[i];
      }
      for ( int i = 0 ; i < ULONGLONG_CONTEXT_LENGTH ; ++i )
      {
        fp_context[i] = rhs.fp_context[i];
      }
      return *this;
    }


    nv_context& operator = (::CONTEXT const& ctx)
    {
      int_context[R0]    = ctx.R0;   
      int_context[R1]    = ctx.R1;   
      int_context[R2]    = ctx.R2;   
      int_context[R3]    = ctx.R3;   
      int_context[R4]    = ctx.R4;   
      int_context[R5]    = ctx.R5;   
      int_context[R6]    = ctx.R6;   
      int_context[R7]    = ctx.R7;   
      int_context[R8]    = ctx.R8;   
      int_context[R9]    = ctx.R9;   
      int_context[R10]   = ctx.R10;  
      int_context[R11]   = ctx.R11;  
      int_context[R12]   = ctx.R12;  
      int_context[SP]    = ctx.Sp;   
      int_context[LR]    = ctx.Lr;   
      int_context[PC]    = ctx.Pc;   
      int_context[CPSR]  = ctx.Cpsr; 
      int_context[FPSCR] = ctx.Fpscr;
      for ( int i = 0 ; i < ULONGLONG_CONTEXT_LENGTH ; ++i )
      {
        fp_context[i] = ctx.D[i];
      }

      return *this;
    }


    void set_r4_r11(::CONTEXT const& ctx)
    {
      int_context[R4]    = ctx.R4;   
      int_context[R5]    = ctx.R5;   
      int_context[R6]    = ctx.R6;   
      int_context[R7]    = ctx.R7;   
      int_context[R8]    = ctx.R8;   
      int_context[R9]    = ctx.R9;   
      int_context[R10]   = ctx.R10;  
      int_context[R11]   = ctx.R11;  
    }


    ::ULONG* r4_r11()
    {
      return &int_context[R4];
    }


    ::ULONG* r15()
    {
      return &int_context[PC];
    }


  private:
    static void* operator new(::size_t);
  };
  COMPILE_TIME_CHECK ( nv_context::ULONG_CONTEXT_LENGTH * sizeof(::ULONG) % 8 == 0           ,  NV_CONTEXT_ULONG_CONTEXT_LENGTH_WRONG_SIZE )
  COMPILE_TIME_CHECK ( nv_context::ULONGLONG_CONTEXT_LENGTH * sizeof(::ULONGLONG) % 8 == 0   ,  NV_CONTEXT_ULONGLONG_CONTEXT_LENGTH_WRONG_SIZE )
  COMPILE_TIME_CHECK ( sizeof(nv_context) % 8 == 0                                           ,  SIZEOF_NV_CONTEXT_WRONG_VALUE )


}  //  namespace aux_



extern "C"
{
//  the stuff placed in the '*.arm.asm' helpers

  int _CPPRTL_execute_frame_handler
  (
    ::EXCEPTION_RECORD    *  exc_rec
  , void                  *  frame
  , ::CONTEXT             *  ctx
  , ::DISPATCHER_CONTEXT  *  dsp_ctx
  );


#ifdef  NT_KERNEL_MODE
  void _CPPRTL_restore_context
  (
    ::aux_::nv_context    *  ctx
  );
#endif

}


namespace cpprtl
{
namespace eh
{
namespace eh_engine
{

////////////////////////////////////////////////////////
////////  EH stack dispatching/unwinding low-level routines
////////////////////////////////////////////////////////

  namespace aux_
  {

    enum
    {
      STACK_MISALIGNMENT = (1 << 3) - 1
    };


    __declspec ( noinline )
    bool stack_walk_impl
    (
      ::EXCEPTION_RECORD      &  original_exc_rec
    , ::CONTEXT               &  ctx1
    , ::aux_::nv_context      &  nv_ctx
    , ::EXCEPTION_RECORD      &  unwind_exc_rec
    )
    {
    //  at this point the context of the throwed function resides in the 'ctx1'
    
    ////  following is the loop traversing the stack frames for:
    ////  1- using the current_exc_rec=original_exc_rec until the frame responsible of the exception handling is found, and then
    ////  2- using the current_exc_rec=unwind_exc_rec to perform unwinding tasks and the stack consolidation
                                          
    
    //  the data used during both the dispatching phase and the unwinding one:
      ::CONTEXT               ctx2;
      ::CONTEXT*              prev_ctx         = &ctx2;
      ::EXCEPTION_RECORD*     current_exc_rec  = &original_exc_rec;
      ::ULONG                 current_frame    = 0;
      ::RUNTIME_FUNCTION*     current_rtf      = 0;
      ::UNWIND_HISTORY_TABLE  uht              = { 0 };
      ::UNWIND_HISTORY_TABLE* current_uht      = &uht;
      image_base_t            image_base       = 0;
      int                     scope_index      = 0;

    //  the data used during the dispatching phase:
      ::ULONG   nested_frame         = 0;
      ::CONTEXT const* original_ctx  = &ctx1;
      *prev_ctx = *original_ctx;
    
    //  the data used during the unwinding phase:
      bool          unwind_phase         = false;
      ::ULONG       unwind_target_frame  = 0;
      ::CONTEXT*    current_ctx          = 0;
      if 
      (
         eh::EXCEPTION_CODE_CPP == original_exc_rec.ExceptionCode
      && EXCEPTION_OPCODE_THROW == original_exc_rec.ExceptionInformation[EXCPTR_OPCODE]
      )
      {
        unwind_exc_rec.ExceptionCode                                     =  eh::EXCEPTION_CODE_CPP;
        unwind_exc_rec.ExceptionInformation[EXCPTR_OPCODE]               =  EXCEPTION_OPCODE_STACKWALKER_UNWIND;
        original_exc_rec.ExceptionInformation[EXCPTR_THR_UNWIND_EXCREC]  =  reinterpret_cast< ::ULONG_PTR>(&unwind_exc_rec);  //  here the frame handler is to save the unwinding info if any
        original_exc_rec.ExceptionInformation[EXCPTR_FLAGS]             |=  EXCEPTION_FLAG_STACKWALKER_UNWIND;
      }

    
      while ( 1 )  //  the frame-traversing loop
      {
        if ( unwind_phase )
        {
          *prev_ctx = *current_ctx;  //  fill in the *prev_ctx with the current context for to be rewound to the previous
        }
    
        IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlLookupFunctionEntry()
        current_rtf = RtlLookupFunctionEntry(prev_ctx->Pc, &image_base, current_uht);
        if ( ! current_rtf )
        {
          if ( 0 == prev_ctx->Pc )
          {
            goto stack_walker_unexpected;
          }
        //  leaf entry
          prev_ctx->Pc  =  prev_ctx->Lr;     //  put the ret address from the link register
          continue;  // the leaf entry have not anything to deal with unwinding 
        }
        else // nested entry
        {
    
          void* frame_handler_data = 0;
          ::size_t control_pc = prev_ctx->Pc;  //  save the currently reached frame instruction pointer
          nv_ctx.set_r4_r11(*prev_ctx);        //  save the currently reached nv-context
    
          IRQL_CHECK ( <=DISPATCH_LEVEL )   //  ::RtlVirtualUnwind()
          ::PEXCEPTION_ROUTINE frame_handler = 
          RtlVirtualUnwind
          (
            unwind_phase ? UNW_FLAG_UHANDLER : UNW_FLAG_EHANDLER  //  let it find the frame_handler
          , image_base
          , control_pc
          , current_rtf
          , prev_ctx
          , &frame_handler_data
          , &current_frame
          , 0  //  we do not care about the volatile context
          );

          if ( current_frame & STACK_MISALIGNMENT )
          {
            goto stack_walker_bad_stack;
          }
    
    
          if ( frame_handler )  //  exception-aware frame is found, let's check it's intentions
          {
stack_walker_restart_frame_loop_for_collided_unwind:

          // preparing the frame handler for invoking
            ::DISPATCHER_CONTEXT dsp_ctx = { 0 };
            dsp_ctx.ControlPc            = control_pc;
            dsp_ctx.ImageBase            = image_base;
            dsp_ctx.FunctionEntry        = current_rtf;
            dsp_ctx.EstablisherFrame     = current_frame;
            dsp_ctx.ContextRecord        = unwind_phase ? current_ctx : prev_ctx;
            dsp_ctx.LanguageHandler      = frame_handler;
            dsp_ctx.HandlerData          = frame_handler_data;
            dsp_ctx.HistoryTable         = current_uht;
            dsp_ctx.ScopeIndex           = scope_index;
            dsp_ctx.ControlPcIsUnwound   = true ;
            dsp_ctx.NonVolatileRegisters = reinterpret_cast< ::UCHAR*>(nv_ctx.r4_r11());
            if ( unwind_phase )
            {
              if ( current_frame > unwind_target_frame )
              {
                goto stack_walker_bad_stack;
              }
    
              if ( current_frame == unwind_target_frame )
              {
                current_exc_rec->ExceptionFlags |= EXCEPTION_TARGET_UNWIND;
              }
            }

          //  do invoke the frame handler
            int disposition = _CPPRTL_execute_frame_handler
            (
              current_exc_rec
            , reinterpret_cast<void*>(current_frame)
            , unwind_phase ? current_ctx : const_cast< ::CONTEXT*>(original_ctx)
            , &dsp_ctx
            );
    
          //  adjust the environment after the frame handler
            if ( nested_frame == current_frame )
            {
              current_exc_rec->ExceptionFlags &= ~EXCEPTION_NESTED_CALL;
              nested_frame = 0;
            }
            if ( unwind_phase)
            {
              current_exc_rec->ExceptionFlags &= ~(EXCEPTION_COLLIDED_UNWIND | EXCEPTION_TARGET_UNWIND);
            }

          //  check the frame disposition
            switch ( disposition )
            {
            case ExceptionContinueExecution:  //  expected at the dispatching and the unwinding, and the actions are similar at both
              if ( current_exc_rec->ExceptionFlags & EXCEPTION_NONCONTINUABLE )
              {
                goto stack_walker_noncontinuable;
              }
              goto stack_walker_continue_execution;
              break;
    

            case ExceptionNestedException:    //  expected only at the dispatching
              current_exc_rec->ExceptionFlags |= EXCEPTION_NESTED_CALL;
              if ( dsp_ctx.EstablisherFrame > nested_frame )
              {
                nested_frame = dsp_ctx.EstablisherFrame;
              }
              continue;

    
            case ExceptionCollidedUnwind:     //  expected both at the dispatching and the unwinding, let's undertake the over-stepping
            //  the upper-frame dispatching loop context is expected to be placed at the moment in our dsp_ctx, so let's continue the current loop from the point reached by the predecessor
              control_pc          =  dsp_ctx.ControlPc;
              image_base          =  dsp_ctx.ImageBase;
              current_rtf         =  dsp_ctx.FunctionEntry;
              current_frame       =  dsp_ctx.EstablisherFrame;
              frame_handler       =  dsp_ctx.LanguageHandler;
              frame_handler_data  =  dsp_ctx.HandlerData;
              current_uht         =  dsp_ctx.HistoryTable;
              scope_index         =  dsp_ctx.ScopeIndex;

              if ( ! unwind_phase )
              {
              //  the frame handler invoked at the dispatching phase is expected to keep the previous frame context within the supplied dispatcher context
                *prev_ctx           =  *dsp_ctx.ContextRecord;
                prev_ctx->Pc        = control_pc;
              }
              else
              {
              //  the frame handler invoked at the unwinding phase is expected to keep the currewnt frame context within the supplied dispatcher context so 
              //  we are to undertake the unwind to the previous frame:
                *current_ctx = *prev_ctx = *dsp_ctx.ContextRecord;
                RtlVirtualUnwind
                (
                  UNW_FLAG_NHANDLER
                , image_base
                , control_pc
                , current_rtf
                , prev_ctx
                , &dsp_ctx.HandlerData                                    // these are already saved in tmp vars so
                , reinterpret_cast< ::ULONG*>(&dsp_ctx.EstablisherFrame)   // we may junk the dsp_ctx
                , 0
                );
                current_exc_rec->ExceptionFlags |= EXCEPTION_COLLIDED_UNWIND;
              }
              goto stack_walker_restart_frame_loop_for_collided_unwind;
              continue;

    
            case ExceptionContinueSearch:     //  expected both at the dispatching and the unwinding 
              if ( STATUS_UNWIND_CONSOLIDATE == unwind_exc_rec.ExceptionCode && !unwind_phase )  //  here to check if the stack unwind should be started or even proceed to the stack consolidation
              {
                if ( EXCEPTION_OPCODE_STACK_CONSOLIDATE == unwind_exc_rec.ExceptionInformation[EXCPTR_OPCODE] )  //  this might to be set at the stack_unwind()
                {
                // make the preparations for the unwind phase:
                  original_ctx = 0;
                  current_ctx  = &ctx1;  //  original context is expected to be here at the moment
                  prev_ctx     = &ctx2;
    
                  unwind_target_frame = unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_TARGET_FRAME];
                  unwind_exc_rec.ExceptionFlags |= EXCEPTION_UNWINDING;  //  'EXCEPTION_EXIT_UNWIND' isn't expected 'cos we have the target frame
                  unwind_exc_rec.ExceptionAddress = reinterpret_cast<void*>(prev_ctx->Pc);
    
                  current_exc_rec = &unwind_exc_rec;
                  nested_frame = 0;
                  unwind_phase = true;  //  the unwind phase can be started only by these eh-lib frame handler
                  continue;             //  follow to the unwinding phase of the stack walking
                }
              }
              if ( unwind_phase )
              {
                if ( current_frame == unwind_target_frame )  //  the target frame has been unwound and we are to perform the stack consolidation
                {
                  goto stack_walker_restore_context;
                }
              //  move the current context up
                ::CONTEXT* tmp_ctx = current_ctx;
                current_ctx  = prev_ctx;
                prev_ctx     = tmp_ctx;
              }
              continue;

    
            default:
              goto stack_walker_invalid_disposition;  //  the frame handler is not expected to return these values
              break;
            };
    
          }
          else if ( unwind_phase )  //  there are no handler but we are to check the target frame and to move the context up
          {
            if ( current_frame == unwind_target_frame )  //  the target frame has been unwound and we are to perform the stack consolidation
            {
              goto stack_walker_restore_context;
            }
          //  move the current context up
            ::CONTEXT* tmp_ctx = current_ctx;
            current_ctx  = prev_ctx;
            prev_ctx     = tmp_ctx;
          }
    
        }
      }


stack_walker_restore_context :
      if ( current_ctx != &ctx1 )
      {
        ctx1 = *current_ctx;
      }
      return true;


stack_walker_continue_execution :
      return false;


stack_walker_noncontinuable:
      unwind_exc_rec.ExceptionCode    = STATUS_NONCONTINUABLE_EXCEPTION;
      unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(unwind_exc_rec);


stack_walker_invalid_disposition:
      unwind_exc_rec.ExceptionCode    = STATUS_INVALID_DISPOSITION;
      unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(unwind_exc_rec);


stack_walker_bad_stack:
      unwind_exc_rec.ExceptionCode    = eh::EXCEPTION_CODE_STACKWALKER_BAD_STACK;
      unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(unwind_exc_rec);


stack_walker_unexpected :
      unwind_exc_rec.ExceptionCode    = eh::EXCEPTION_CODE_STACKWALKER_UNEXPECTED_RETURN;
      unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(unwind_exc_rec);


      return false;  //  could anyone achieve here ?

    }


  }  //  namespace aux_



#ifndef  NT_KERNEL_MODE

  //  do NOT use the objects with destructor semantic at this scope because of 'RtlRestoreContext()' just cuts the stack up
  __declspec ( noinline )
  void stack_walk(::EXCEPTION_RECORD& original_exc_rec)
  {
  //  the data to be used in the stack_walk_impl()
    ::CONTEXT               ctx1;                    //  keep at this scope for using with 'RtlRestoreContext()'
    ::EXCEPTION_RECORD      unwind_exc_rec = { 0 };  //  ^
    ::aux_::nv_context      nv_ctx;
  
    IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlCaptureContext()
    RtlCaptureContext(&ctx1);

    ::ULONG              frame = 0;
    ::RUNTIME_FUNCTION*  rtf   = 0;
    image_base_t         image_base   = 0;
    void*                handler_data = 0;

    IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlLookupFunctionEntry()
    rtf = RtlLookupFunctionEntry(ctx1.Pc, &image_base, 0);
    if ( ! rtf )
    {
      unwind_exc_rec.ExceptionCode    = eh::EXCEPTION_CODE_STACKWALKER_UNEXPECTED_LEAF_ENTRY;
      unwind_exc_rec.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
      unwind_exc_rec.ExceptionRecord  = &original_exc_rec;
      unwind_exc_rec.NumberParameters = 0;
      eh::aux_::raise_exception(unwind_exc_rec);
    }
    IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlVirtualUnwind()
    RtlVirtualUnwind
    (
      UNW_FLAG_NHANDLER  //  skip the frame_handler searching
    , image_base
    , ctx1.Pc
    , rtf
    , &ctx1
    , &handler_data
    , &frame
    , 0
    );

    original_exc_rec.ExceptionAddress = reinterpret_cast<void*>(ctx1.Pc);
  //  at this point the context of the throwed function now we have got in the 'ctx1'

    if ( aux_::stack_walk_impl(original_exc_rec, ctx1, nv_ctx, unwind_exc_rec) )
    {
    //  the stack_walk_impl() has got the stack unwound and has filled the 'ctx1' and the 'unwind_exc_rec' with the proper consolidation data
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_ARM_NV_CONTEXT] = reinterpret_cast< ::ULONG_PTR>(nv_ctx.r4_r11());
      ctx1.Pc = (reinterpret_cast< ::ULONG (*)(::EXCEPTION_RECORD*)>(unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_CALLBACK_ADDR]))(&unwind_exc_rec);
    //  just doesn't return into this scope
      RtlRestoreContext(&ctx1, 0);
    }
    //  otherwise the stack_walk_impl() requests for continuing the stack_walk() caller
    return;
  }



#else  //  NT_KERNEL_MODE

  namespace aux_
  {

    void stack_walk_rewind_context
    (
      ::CONTEXT& ctx
    , ::EXCEPTION_RECORD& er
    , int unwind_scope_depth
    )
    {
      ::ULONG              frame = 0;
      image_base_t         image_base   = 0;
      void*                handler_data = 0;
      do
      {
        IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlLookupFunctionEntry()
        ::RUNTIME_FUNCTION*  rtf = RtlLookupFunctionEntry(ctx.Pc, &image_base, 0);
        if ( ! rtf )
        {
          er.ExceptionCode    = eh::EXCEPTION_CODE_STACKWALKER_UNEXPECTED_LEAF_ENTRY;
          er.ExceptionFlags  |= EXCEPTION_NONCONTINUABLE;
          er.ExceptionRecord  = 0;
          er.NumberParameters = 0;
          eh::aux_::raise_exception(er);
        }
        IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlVirtualUnwind()
        RtlVirtualUnwind
        (
          UNW_FLAG_NHANDLER  //  skip the frame_handler searching
        , image_base
        , ctx.Pc
        , rtf
        , &ctx
        , &handler_data
        , &frame
        , 0
        );
      }
      while ( unwind_scope_depth-- > 0 );
    }


    __declspec ( noinline )
    bool stack_walk_prepare_and_run
    (
      ::EXCEPTION_RECORD      &  original_exc_rec
    , ::aux_::nv_context      &  nv_ctx
    , ::EXCEPTION_RECORD      &  unwind_exc_rec
    , int                        unwind_scope_depth
    )
    {
    //  the data to be used in the stack_walk_impl()
      ::CONTEXT  ctx1;

      IRQL_CHECK ( <= DISPATCH_LEVEL );  //  RtlCaptureContext()
      RtlCaptureContext(&ctx1);
      stack_walk_rewind_context(ctx1, unwind_exc_rec, unwind_scope_depth);  //  get the context record of the 'stack_walk()' caller
      original_exc_rec.ExceptionAddress = reinterpret_cast<void*>(ctx1.Pc);
    //  at this point the context of the throwed function now we have got in the 'ctx1'

      bool res = stack_walk_impl(original_exc_rec, ctx1, nv_ctx, unwind_exc_rec);
      if ( res )
      {
        nv_ctx = ctx1;  //  save all the necessary context items for the 'stack_walk()' to be able to perform continuation duties
      }
      return res;
    }


  }  //  namespace aux_


  //  do NOT use the objects with destructor semantic at this scope because of '_CPPRTL_restore_context()' just cuts the stack up
  __declspec ( noinline )
  void stack_walk(::EXCEPTION_RECORD& original_exc_rec)
  {
    ::EXCEPTION_RECORD      unwind_exc_rec = { 0 };  //  the data to be used in the stack_walk_impl()
    ::aux_::nv_context      nv_ctx;                  //  ^ 

    if ( aux_::stack_walk_prepare_and_run(original_exc_rec, nv_ctx, unwind_exc_rec, 1) )
    {
    //  the stack_walk_prepare_and_run() has got the stack unwound and has filled the 'nv_ctx' and the 'unwind_exc_rec' with the proper consolidation data
      unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_ARM_NV_CONTEXT] = reinterpret_cast< ::ULONG_PTR>(nv_ctx.r4_r11());
      *nv_ctx.r15() = (reinterpret_cast< ::ULONG (*)(::EXCEPTION_RECORD*)>(unwind_exc_rec.ExceptionInformation[EXCPTR_UNW_CALLBACK_ADDR]))(&unwind_exc_rec);
    //  just doesn't return into this scope
      _CPPRTL_restore_context(&nv_ctx);
    }
    //  otherwise we are being requested for continuing the stack_walk() caller
    return;
  }


#endif  //  NT_KENREL_MODE


}  //  namespace eh_engine
}  //  namespace eh
}  //  namespace cpprtl


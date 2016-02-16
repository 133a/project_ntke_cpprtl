/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing nested seh-exceptions by RtlRaiseException().
////  in this test the 'nested' exception is raised from the scope of __except{} block filter when
////  the stack consolidation is not yet performed by the C_cpecific_handler()/except_handler3() so
////  those are true nested exceptions and the stack is mainly exhausted by the recursive invokes of
////  the RtlDispatchException()
////
////  MT-safe
/////////////////////////////////////////////


#ifdef NT_KERNEL_MODE

  #if DDK_VER < 6000
    extern "C"
    {
    #include <wdm.h>
    }
  #else
    #include <wdm.h>
  #endif

  extern "C" void __stdcall RtlRaiseException(::EXCEPTION_RECORD* ExceptionRecord);

#else // ! NT_KERNEL_MODE

  #  include <windows.h>

#endif // NT_KERNEL_MODE


#include <excpt.h>
#include "context.h"


namespace
{
  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    UNEXPECTED_CATCH1         = -2,
    UNEXPECTED_CATCH2         = -3,
    UNEXPECTED_CATCH3         = -4,
    RAISE21                   = 0xE0000000 + 21,

#ifdef NT_KERNEL_MODE
  //  here we just get stack overflow if too many nested raises occures in kernel mode
  #if defined (_M_IX86)
    NESTED_RAISE_STATUS_LIMIT21      = 2,  // '4' leads to BSOD (x86 mscl), '3' (x86_nt5.1 kAPC icl -Od)
  #elif defined (_M_AMD64) || defined (_M_X64) || defined (_M_ARM)
    NESTED_RAISE_STATUS_LIMIT21      = 4,  // '6' leads to BSOD (x64) ('5' kAPC)
  #else
  #  error check $(target.arch)
  #endif
#else
  //  feel more free to make nested raises in a user mode process
    NESTED_RAISE_STATUS_LIMIT21      = 128,
#endif  //  NT_KERNEL_MODE
  };
}


namespace
{

  int nested_raise_function(context& ctx)
  {
    __try
    {
      __try
      {
        if ( ctx.state < NESTED_RAISE_STATUS_LIMIT21  )
        {
          ++ctx.state;
          EXCEPTION_RECORD exc_rec = { 0 };
        #ifdef NT_KERNEL_MODE
          exc_rec.ExceptionCode = RAISE21;
          RtlRaiseException(&exc_rec);
        #else
          RaiseException(RAISE21, 0, 0, 0);
        #endif 
        }
      }
      __except ( RAISE21 == GetExceptionCode() ? nested_raise_function(ctx) : EXCEPTION_CONTINUE_SEARCH )
      {
        __try
        {
          nested_raise_function(ctx);
        }
        __except ( EXCEPTION_EXECUTE_HANDLER )
        {
          ctx.state = UNEXPECTED_CATCH3;
        }
      }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
      ctx.state = UNEXPECTED_CATCH2;
    }
    return EXCEPTION_EXECUTE_HANDLER;
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test21()
  {
#ifdef NT_KERNEL_MODE
    if ( KeGetCurrentIrql() > DISPATCH_LEVEL )
    {
    //  in kernel the RtlRaiseException() is only allowed when IRQL<=DISPATCH_LEVEL otherwise a bound trap is expected
      return 0;
    }
#endif  //  NT_KERNEL_MODE

    context ctx(NESTED_RAISE_STATUS_LIMIT21);
    ctx.state = EH_OK;

    __try
    {
      nested_raise_function(ctx);
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

    return ctx.balance();
  }

}  }  }


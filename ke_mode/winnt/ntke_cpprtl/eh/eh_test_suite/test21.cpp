/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing nested seh-exceptions by RtlRaiseException().
////  in this test the 'nested' exception is raised from the scope of __except{} block filter when
////  the stack consolidation hasn't yet performed by the C_cpecific_handler()/except_handler3() so
////  the stack is mainly exhausted by the recursive invokes of RtlDispatchException()
////
////  MT-safe
/////////////////////////////////////////////


#ifdef NT_KERNEL_MODE
# if DDK_VER < 6000
  extern "C"
  {
    #include <ntddk.h>
  }
# else
  #include <ntddk.h>
# endif
#endif

#include <excpt.h>
#include "context.h"


extern "C"
{
#ifdef NT_KERNEL_MODE
  void __stdcall RtlRaiseException(::EXCEPTION_RECORD* ExceptionRecord);
#else
  void __stdcall RaiseException(int code, int flags, int nparam, void** params);
#endif
}


namespace
{
  enum
  {
    EH_OK              = 0,
    UNEXPECTED_CATCH1  = -2,
    UNEXPECTED_CATCH2  = -3,
    RAISE21            = 0xE0000000 + 21,
  };

  enum
  {
  #ifdef NT_KERNEL_MODE
    // here we just get stack overflow if too many nested raises occures in kernel mode
    #if defined (_M_IX86)
      NESTED_RAISE_STATUS_LIMIT21      = 2,  // '4' leads to BSOD (x86 mscl), '3' (x86_nt5.1 kAPC icl -Od)
    #elif defined (_M_AMD64) || defined (_M_X64) || defined (_M_ARM)
      NESTED_RAISE_STATUS_LIMIT21      = 4,  // '6' leads to BSOD (x64) ('5' kAPC)
    #else
    #  error check $(target.arch)
    #endif
  #else
    // feel free to make nested raises in a user mode process
      NESTED_RAISE_STATUS_LIMIT21      = 128,
  #endif  // NT_KERNEL_MODE
  };


  int nested_raise_function(context& ctx)
  {
    __try
    {
      __try
      {
        if ( ++ctx.state < NESTED_RAISE_STATUS_LIMIT21 )
        {
        #ifdef NT_KERNEL_MODE
          EXCEPTION_RECORD exc_rec = { 0 };
          exc_rec.ExceptionCode   = RAISE21;
          exc_rec.ExceptionFlags |= EXCEPTION_NONCONTINUABLE;
          RtlRaiseException(&exc_rec);
        #else
          RaiseException(RAISE21, 0, 0, 0);
        #endif 
        }
      }
      __except ( RAISE21 == GetExceptionCode() ? nested_raise_function(ctx) : EXCEPTION_CONTINUE_SEARCH )
      {
      }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
      ctx.state = UNEXPECTED_CATCH2;
    }
    return EXCEPTION_EXECUTE_HANDLER;
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test21()
  {
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


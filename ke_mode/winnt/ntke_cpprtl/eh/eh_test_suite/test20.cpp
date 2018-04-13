/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing nested seh-exceptions by RtlRaiseException().
////  in this test the 'nested' exception is raised from the scope of __except{} block after
////  the stack consolidation has been performed by the C_cpecific_handler()/except_handler3() so
////  those are pseudo nested exceptions and the stack is mainly exhausted by the 
////  EXCEPTION_RECORD placing in the recursive 'nested_raise_function()' frames
////
////  MT-safe
/////////////////////////////////////////////


#ifdef NT_KERNEL_MODE
#  include "ntddk.include.h"
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
    EH_OK              = 0
  , UNEXPECTED_CATCH1  = 201
  , UNEXPECTED_CATCH2  = 202
  , RAISE20            = 0xE0000000 + 20
  };

  enum
  {
  #ifdef NT_KERNEL_MODE
    // here we just get stack overflow if too many nested raises occures in kernel mode
    NESTED_RAISE_STATUS_LIMIT20      = 16,
  #else
    // feel more free to make nested raises in a user mode process
    NESTED_RAISE_STATUS_LIMIT20      = 256,
  #endif  // NT_KERNEL_MODE
  };


  void nested_raise_function(context& ctx)
  {
    __try
    {
      __try
      {
        if ( ++ctx.state < NESTED_RAISE_STATUS_LIMIT20 )
        {
        #ifdef NT_KERNEL_MODE
          EXCEPTION_RECORD exc_rec = { 0 };
          exc_rec.ExceptionCode   = RAISE20;
          exc_rec.ExceptionFlags |= EXCEPTION_NONCONTINUABLE;
          RtlRaiseException(&exc_rec);
        #else
          RaiseException(RAISE20, 0, 0, 0);
        #endif 
        }
      }
      __except ( RAISE20 == GetExceptionCode() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
      {
        nested_raise_function(ctx);
      }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
      ctx.state = UNEXPECTED_CATCH2;
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test20()
  {
    context ctx(NESTED_RAISE_STATUS_LIMIT20);
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


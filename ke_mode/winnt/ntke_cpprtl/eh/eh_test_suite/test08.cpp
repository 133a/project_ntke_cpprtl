/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing nested exceptions
////  MT-safe
/////////////////////////////////////////////


#include "context.h"
#include "eh_config.h"


namespace
{

  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    UNEXPECTED_CATCH1         = -2,
    UNEXPECTED_CATCH2         = -3,
    UNEXPECTED_CATCH3         = -4,
    UNEXPECTED_CATCH4         = -5,
    UNEXPECTED_CATCH5         = -6,
    UNEXPECTED_CATCH6         = -7,
    UNEXPECTED_CATCH7         = -8,
    UNEXPECTED_CATCH8         = -9,
    THROW8                    = 35,

#ifdef NT_KERNEL_MODE   // here we just get the stack overrun (STOP-0x7F-UNEXPECTED_KERNEL_MODE_TRAP-EXCEPTION_DOUBLE_FAULT) if 
                        // more than 'NESTED_THROW_LIMIT8' nested throws occure in kernel mode.
                        // the stack for kernel threads is too small (x86-12k, x64-24k, arm-12k)

  #ifdef  CFG_EH_STACK_WALKER  // the eh lib's 'stack_walk()' is used

    #  if   defined     ( __ICL )                            //  for icl
    #    if    defined  ( _M_X64 ) || defined ( _M_AMD64 )   //    x64 eh lib is used with icl
            NESTED_THROW_LIMIT8     = 7,                     //      '10'- stack overrun (x64) ('9' kAPC)
    #    elif  defined  ( _M_IX86 )                          //    x86 eh lib is used with icl
            NESTED_THROW_LIMIT8     = 14,                    //      '16'- stack overrun (x86)
    #    else
    #       error check $(target.arch)
    #    endif

    #  elif defined     ( _MSC_VER )                         //  for mscl
    #    if    defined  ( _M_X64 ) || defined ( _M_AMD64 )   //    x64 eh lib is used with mscl
            NESTED_THROW_LIMIT8     = 7,                     //      '9'- stack overrun (x64) ('8' kAPC)
    #    elif  defined  ( _M_IX86 )                          //    x86 eh lib is used with mscl
            NESTED_THROW_LIMIT8     = 16,                    //      '19'- stack overrun (x86)
    #    elif  defined  ( _M_ARM )                           //    arm eh lib is used with mscl
            NESTED_THROW_LIMIT8     = 8,                     //      '10'- stack overrun (arm PANIC_STACK_SWITCH)
    #    else
    #       error check $(target.arch)
    #    endif                                             
    #  endif  // compiler

  #else     // external dispatching/unwinding engines are used
          NESTED_THROW_LIMIT8     = 1,   // '2'- stack overrun
  #endif    // CFG_EH_STACK_WALKER


#else       // feel free to make nested rethrows in a user mode process
    NESTED_THROW_LIMIT8       = 32,
#endif      // NT_KERNEL_MODE
  };

}


namespace
{

  void nested_rethrow_function(context& ctx)
  {
    try
    {
      eh_test et(ctx);
      if ( ctx.state < NESTED_THROW_LIMIT8  )
      {
        ++ctx.state;
        throw;
      }
    }
    catch (int)
    {
      eh_test et(ctx);
      try
      {
        eh_test et(ctx);
        nested_rethrow_function(ctx);
      }
      catch ( ... )
      {
        ctx.state = UNEXPECTED_CATCH8;
      }
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH7;
    }
  }

}


namespace
{

  void nested_throw_function(context& ctx)
  {
    try
    {
      eh_test et(ctx);
      if ( ctx.state < NESTED_THROW_LIMIT8 * 2 )  //  
      {
        ++ctx.state;
        throw eh_test(ctx);
      }
    }
    catch (eh_test ex)
    {
      eh_test et(ctx);
      try
      {
        eh_test et(ctx);
        nested_throw_function(ctx);
      }
      catch ( ... )
      {
        ctx.state = UNEXPECTED_CATCH6;
      }
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH5;
    }
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test08()
  {
    context ctx(NESTED_THROW_LIMIT8 * 2);  // 2 passes - the first one for nested rethrows, the second for nested 'throw T'
    ctx.state = EH_OK;

    try
    {
      throw int(THROW8);
    }
    catch ( int )
    {
      try
      {
        nested_rethrow_function(ctx);
      }
      catch ( ... )
      {
        ctx.state = UNEXPECTED_CATCH4;
      }
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH3;
    }


    try
    {
      throw int(THROW8);
    }
    catch ( int )
    {
      try
      {
        nested_throw_function(ctx);
      }
      catch ( ... )
      {
        ctx.state = UNEXPECTED_CATCH2;
      }
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

    return ctx.balance();

  }

}  }  }


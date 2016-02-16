/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing UDT rethrow
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    UNEXPECTED_CATCH1         = -2,
    UNEXPECTED_CATCH2         = -3,
    MAGIC_VALUE10             = 874543,
    MAGIC_VALUE10_2           = 738,
  };
}


namespace
{

  void ff_throw(context& ctx)
  {
    exc_test t1(ctx, MAGIC_VALUE10_2);
    try
    {
      exc_test t2(ctx, MAGIC_VALUE10_2);
      throw ; // !
    }
    catch (int)
    {
      exc_test t3(ctx, MAGIC_VALUE10_2);
      ctx.state = UNEXPECTED_CATCH2;
    }
    catch (exc_test ex)
    {
      exc_test t4(ctx, MAGIC_VALUE10_2);
      exc_test t5(ctx, MAGIC_VALUE10_2);
      ctx.state = ex.i;
    }
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test10()
  {
    context ctx(MAGIC_VALUE10);
    {
      exc_test t1(ctx, MAGIC_VALUE10_2);
      try
      {
        exc_test t2(ctx, MAGIC_VALUE10_2);
        throw exc_test(ctx, MAGIC_VALUE10);  //  this magic value is expected to be catched in ff_throw() and captured in context::state
      }
      catch (exc_test ex)
      {
        exc_test t3(ctx, MAGIC_VALUE10_2);
        try 
        {
          exc_test t4(ctx, MAGIC_VALUE10_2);
          ff_throw(ctx);
        }
        catch (exc_test ex)
        {
          exc_test t5(ctx, MAGIC_VALUE10_2);
          ctx.state = UNEXPECTED_CATCH1;
        }
      }
    }
    return ctx.balance();
  }

}  }  }


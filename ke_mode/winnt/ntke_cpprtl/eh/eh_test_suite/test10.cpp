/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
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
    EH_OK              = 0,
    UNEXPECTED_CATCH1  = -2,
    UNEXPECTED_CATCH2  = -3,
    MAGIC_VALUE10      = 874543,
    MAGIC_VALUE10_2    = 738,
  };


  void ff_throw(context& ctx)
  {
    eh_test t1(ctx, MAGIC_VALUE10_2);
    try
    {
      eh_test t2(ctx, MAGIC_VALUE10_2);
      throw ;  // !
    }
    catch (int)
    {
      eh_test t3(ctx, MAGIC_VALUE10_2);
      ctx.state = UNEXPECTED_CATCH2;
    }
    catch (eh_test ex)
    {
      eh_test t4(ctx, MAGIC_VALUE10_2);
      eh_test t5(ctx, MAGIC_VALUE10_2);
      ctx.state = ex.i;
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test10()
  {
    context ctx(MAGIC_VALUE10);
    {
      eh_test t1(ctx, MAGIC_VALUE10_2);
      try
      {
        eh_test t2(ctx, MAGIC_VALUE10_2);
        throw eh_test(ctx, MAGIC_VALUE10);  // this magic value is expected to be catched in ff_throw() and captured in context::state
      }
      catch (eh_test ex)
      {
        eh_test t3(ctx, MAGIC_VALUE10_2);
        try 
        {
          eh_test t4(ctx, MAGIC_VALUE10_2);
          ff_throw(ctx);
        }
        catch (eh_test ex)
        {
          eh_test t5(ctx, MAGIC_VALUE10_2);
          ctx.state = UNEXPECTED_CATCH1;
        }
      }
    }
    return ctx.balance();
  }

}  }  }


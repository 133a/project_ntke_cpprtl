/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing UDT& rethrow
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK              = 0
  , UNEXPECTED_CATCH1  = 111
  , UNEXPECTED_CATCH2  = 112
  , MAGIC_VALUE11      = 113
  , MAGIC_VALUE11_2    = 114
  };


  void ff_throw(context& ctx)
  {
    eh_test t1(ctx, MAGIC_VALUE11_2);
    try
    {
      eh_test t2(ctx, MAGIC_VALUE11_2);
      throw ;  // !
    }
    catch (int&)
    {
      eh_test t3(ctx, MAGIC_VALUE11_2);
      ctx.state = UNEXPECTED_CATCH2;
    }
    catch (eh_test& ex)
    {
      eh_test t4(ctx, MAGIC_VALUE11_2);
      eh_test t5(ctx, MAGIC_VALUE11_2);
      ctx.state = ex.i;
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test11()
  {
    context ctx(MAGIC_VALUE11);
    {
      eh_test t1(ctx, MAGIC_VALUE11_2);
      try
      {
        eh_test t2(ctx, MAGIC_VALUE11_2);
        throw eh_test(ctx, MAGIC_VALUE11);  // this magic value is expected to be catched in ff_throw() and captured in context::state
      }
      catch (eh_test&)
      {
        eh_test t3(ctx, MAGIC_VALUE11_2);
        try 
        {
          eh_test t4(ctx, MAGIC_VALUE11_2);
          ff_throw(ctx);
        }
        catch (eh_test&)
        {
          eh_test t5(ctx, MAGIC_VALUE11_2);
          ctx.state = UNEXPECTED_CATCH1;
        }
      }
    }
    return ctx.balance();
  }

}  }  }


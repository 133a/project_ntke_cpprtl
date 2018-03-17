/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing the living time of the current exception
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
    UNEXPECTED_CATCH3         = -4,
    SPECIAL_EXCEPTION26       = 26,
    UNEXPECTED_VALUE26        = -111,
  };
}


namespace
{

  void f26_1(context& ctx)
  {
    try
    {
      eh_test et(ctx);
      throw;
    }
    catch (eh_test const& ex)
    {
      eh_test et(ctx);
      ctx.state += ex.i;
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH3;
    }
  }

  void f26_2(context& ctx)
  {
    try
    {
      eh_test et(ctx);
      throw eh_test(ctx, SPECIAL_EXCEPTION26);
    }
    catch (eh_test const& ex)
    {
      eh_test et(ctx);
      ctx.state += ex.i;
      f26_1(ctx);
      throw;
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH2;
    }
  }

  void f26_3(context& ctx)
  {
    eh_test et1(ctx, UNEXPECTED_VALUE26);
    eh_test et2(ctx, UNEXPECTED_VALUE26);
    eh_test et3(ctx, UNEXPECTED_VALUE26);
    eh_test et4(ctx, UNEXPECTED_VALUE26);
    eh_test et5(ctx, UNEXPECTED_VALUE26);
    eh_test et6(ctx, UNEXPECTED_VALUE26);
    eh_test et7(ctx, UNEXPECTED_VALUE26);
    eh_test et8(ctx, UNEXPECTED_VALUE26);
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test26()
  {
    context ctx ( SPECIAL_EXCEPTION26 * 3 );  // 3 catches are expected
    ctx.state = EH_OK;

    try
    {
      f26_2(ctx);
    }
    catch (eh_test const& ex)
    {
      f26_3(ctx);
      ctx.state += ex.i;
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

    return ctx.balance();
  }

}  }  }


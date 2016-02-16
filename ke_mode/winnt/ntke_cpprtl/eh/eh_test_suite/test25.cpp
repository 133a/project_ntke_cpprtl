/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing the stack unwinding
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
    SPECIAL_EXCEPTION25       = 25,
    RECURSION_COUNT25         = 16,
  };
}


namespace
{
  void f25(context& ctx)
  {
    exc_test et(ctx);
    if ( ++ctx.state >= RECURSION_COUNT25 )
    {
      exc_test et(ctx);
      throw int(SPECIAL_EXCEPTION25);
    }
    f25(ctx);
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test25()
  {
    context ctx ( RECURSION_COUNT25 + SPECIAL_EXCEPTION25 );  //
    ctx.state = EH_OK;

    try
    {
      exc_test et(ctx);
      f25(ctx);
    }
    catch (int const i)
    {
      ctx.state += i;
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

    return ctx.balance();
  }

}  }  }


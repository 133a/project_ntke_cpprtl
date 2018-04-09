/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing a function-try-block
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK                = 0,
    UNEXPECTED_CATCH1    = -2,
    UNEXPECTED_CATCH2    = -3,
    UNEXPECTED_CATCH3    = -4,
    UNEXPECTED_CATCH4    = -5,
    UNEXPECTED_RET1      = -10,
    UNEXPECTED_RET2      = -11,
    SPECIAL_EXCEPTION23  = 23,
  };


  int ftb23(context& ctx)
  try
  {
    eh_test et(ctx);
    throw int(SPECIAL_EXCEPTION23);
    return UNEXPECTED_RET1;
  }
  catch (int i)
  {
    eh_test et(ctx);
    if ( SPECIAL_EXCEPTION23 == i)
    {
      ctx.state += i;
    }
    else
    {
      ctx.state = UNEXPECTED_CATCH4;
    }
    return i;
  }
  catch (...)
  {
    ctx.state = UNEXPECTED_CATCH3;
    return UNEXPECTED_RET2;
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test23()
  {
    context ctx ( SPECIAL_EXCEPTION23 * 2 );  // 1 catch and 1 return value are expected
    ctx.state = EH_OK;
    {
      eh_test et(ctx);
      try
      {
        eh_test et(ctx);
        ctx.state += ftb23(ctx);
      }
      catch (int)
      {   
        ctx.state = UNEXPECTED_CATCH2;
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_CATCH1;
      }
    }
    return ctx.balance();
  }

}  }  }


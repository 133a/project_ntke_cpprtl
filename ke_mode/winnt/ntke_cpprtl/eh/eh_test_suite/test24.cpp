/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing an execution flow moving out of a catch-block
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
    SPECIAL_EXCEPTION24  = 24,
    CYCLE_COUNT24        = 16,
  };
}


namespace cpprtl { namespace test { namespace eh
{

  int test24()
  {
    context ctx(CYCLE_COUNT24);  //
    ctx.state = EH_OK;
    try
    {
      eh_test et(ctx);
      while ( ctx.state < CYCLE_COUNT24 )
      {
        eh_test et(ctx);
        try
        {
          eh_test et(ctx);
          throw int(SPECIAL_EXCEPTION24);
        }
        catch (int i)
        {   
          eh_test et(ctx);
          if ( SPECIAL_EXCEPTION24 == i )
          {
            ++ctx.state;
            continue;
          }
          ctx.state = UNEXPECTED_CATCH3;
          break;
        }
        catch (...)
        {
          ctx.state = UNEXPECTED_CATCH2;
          break;
        }
      }
    }
    catch (...)
    {
      ctx.state = UNEXPECTED_CATCH1;
    }
    return ctx.balance();
  }

}  }  }


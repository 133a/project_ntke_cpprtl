/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing a try-catch block rethrow in another catch scope
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
    UNEXPECTED_CATCH4         = -5,
    MAGIC_VALUE13             = 7653,
  };
}


namespace cpprtl { namespace test { namespace eh
{

  int test13()
  {
    context ctx(MAGIC_VALUE13);
    {
      exc_test et(ctx);
      try
      {
        exc_test et(ctx);
        try 
        {
          exc_test et(ctx);
          throw exc_test(ctx, MAGIC_VALUE13);
        }
        catch (exc_test& ex)
        {
          exc_test et(ctx);
          if (MAGIC_VALUE13 == ex.i)
          {
            try
            {
              exc_test et(ctx);
              throw;
            }
            catch (exc_test/*&*/ ex)
            {
              exc_test et(ctx);
              ctx.state = ex.i;
            }
            catch ( ... )
            {
              ctx.state = UNEXPECTED_CATCH4;
            }
          }
          else
          {
            ctx.state = UNEXPECTED_CATCH3;
          }
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
    }
    return ctx.balance();
  }

}  }  }


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing a try-catch block throw in another catch scope
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
    MAGIC_VALUE19             = 5648,
  };
}


namespace cpprtl { namespace test { namespace eh
{

  int test19()
  {
    context ctx(MAGIC_VALUE19);
    {
      eh_test et(ctx);
      try
      {
        eh_test et(ctx);
        try 
        {
          eh_test et(ctx);
          throw eh_test(ctx, MAGIC_VALUE19);
        }
        catch (eh_test ex)
        {
          eh_test et(ctx);
          if (MAGIC_VALUE19 == ex.i)
          {
            try
            {
              eh_test et(ctx);
              throw eh_test(ctx, MAGIC_VALUE19);
            }
            catch (eh_test ex)
            {
              eh_test et(ctx);
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


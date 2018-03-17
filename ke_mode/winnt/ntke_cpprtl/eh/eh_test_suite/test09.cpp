/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing built-in type rethrow
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
    THROW09                   = 123,
  };
}


namespace
{

  void ff_throw(context& ctx)
  {
    try
    {
      throw ; // !
    }
    catch (float)
    {
      ctx.state = UNEXPECTED_CATCH2;
    }
    catch (int i)
    {
      ctx.state = i;
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test09()
  {
    context ctx(THROW09);

    try
    {
      throw int(THROW09);
    }
    catch (int)
    {
      try 
      {
        ff_throw(ctx);
      }
      catch (int)
      {
        ctx.state = UNEXPECTED_CATCH1;
      }
    }
  
    return ctx.balance();
  }

}  }  }


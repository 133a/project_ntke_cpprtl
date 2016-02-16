/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
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
    UNEXPECTED_CATCH          = -2,
    INIT07                    = 20,
    THROW07                   = 30,
    RETURN1                   = 111,
    RETURN2                   = 232,
  };
}


namespace
{

  int f2()
  {
    throw int(THROW07);
    return RETURN2;
  }


  int f1()
  {
    try
    {
      int a = f2();
    }
    catch (int)
    {
      throw;
    }
    return RETURN1;
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test07()
  {
    context ctx(INIT07 + THROW07) ;
    ctx.state = INIT07;
  
    try
    {
      ctx.state = f1();
    }
    catch (int i) // THROW07 is expected
    {
      ctx.state += i;
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH; 
    }
  
    return ctx.balance();
  }

}  }  }




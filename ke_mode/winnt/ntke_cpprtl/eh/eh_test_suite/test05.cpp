/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing stack cleanup
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    MAGIC_VALUE5              = 235,
  };
}


namespace
{

  struct ctest05
    : protected eh_test
  {
    ctest05(context& c) 
      : eh_test ( c )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }


    ctest05(ctest05 const& src)
      : eh_test  ( src )
    {
      ++ctx.cctor_count;
      ++ctx.xtor_count;
    }


    ~ctest05()
    {
      ctx.state += MAGIC_VALUE5;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }

  };

}


namespace cpprtl { namespace test { namespace eh
{

  int test05()
  {
    context ctx(MAGIC_VALUE5 * 2);  // one dtor + one catched magic value
    ctx.state = EH_OK;

    try
    {
      ctest05 dtor_tst(ctx);
      throw int(MAGIC_VALUE5);
    }
    catch (int ex)
    {
      ctx.state += ex;
    }
  
    return ctx.balance();
  }

}  }  }


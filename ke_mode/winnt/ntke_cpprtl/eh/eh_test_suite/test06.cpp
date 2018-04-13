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
    EH_OK         = 0
  , WRONG_STATE   = 60
  , MAGIC_VALUE6  = 66
  };


  struct ctest06
    : protected eh_test
  {
    ctest06(context& c) 
      : eh_test ( c )
    {}

    ctest06(ctest06 const& src)
      : eh_test ( src )
    {}

    ~ctest06()
    {
      ctx.state += MAGIC_VALUE6;
    }
  };


  int f2(context& ctx)
  {
    ctest06 dtor_tst(ctx);
    throw int(MAGIC_VALUE6);
    return WRONG_STATE;
  }

  int f1(context& ctx)
  {
    ctest06 dtor_tst(ctx);
    return f2(ctx);
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test06()
  {
    context ctx(MAGIC_VALUE6 * 4);  // 3 dtors + 1 catched magic value
    ctx.state = EH_OK;
  
    try
    {
      ctest06 dtor_tst(ctx);
      ctx.state = f1(ctx);
    }
    catch (int ex)
    {
      ctx.state += ex;
    }
  
    return ctx.balance();
  }

}  }  }


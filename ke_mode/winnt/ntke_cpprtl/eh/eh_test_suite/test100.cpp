/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing a try-ctach class constructing scope (function-try-block)
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK                 = 0,
    UNEXPECTED_CATCH1     = -2,
    UNEXPECTED_CATCH2     = -3,
    UNEXPECTED_CATCH3     = -4,
    UNEXPECTED_CATCH4     = -5,
    UNEXPECTED_CATCH5     = -6,
    UNEXPECTED_CATCH6     = -7,
    SPECIAL_EXCEPTION100  = 100,
    MAGIC_DTOR100         = 1010,
  };


  class base00
  {
    context& ctx;

  public:
    explicit base00(context& c_)
      : ctx  ( c_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    ~base00()
    {
      ctx.state += MAGIC_DTOR100;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  class base01
    : public base00
  {
    context& ctx;

  public:
    base01(context& c_)
    try
      : base00  ( c_ )
      , ctx     ( c_ )
    {
      eh_test et(c_);
      throw int(SPECIAL_EXCEPTION100);
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }
    catch (int i)
    {
      eh_test et(c_);
      if ( SPECIAL_EXCEPTION100 == i )
      {
        eh_test et(c_);
        c_.state += i;
      }
      else
      {
        c_.state = UNEXPECTED_CATCH6;
      }
      throw;
    }
    catch (...)
    {
      c_.state = UNEXPECTED_CATCH5;
    }

    ~base01()
    {
      ctx.state += MAGIC_DTOR100;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  struct ctest100
    : public base01
  {
    context& ctx;

    ctest100(context& c_) 
    try
      : base01  ( c_ )
      , ctx     ( c_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }
    catch (int i)
    {
      eh_test et(c_);
      if ( SPECIAL_EXCEPTION100 == i )
      {
        eh_test et(c_);
        c_.state += i;
      }
      else
      {
        c_.state = UNEXPECTED_CATCH4;
      }
    //  implicit 'throw;' is here
    }
    catch (...)
    {
      c_.state = UNEXPECTED_CATCH3;
    }

    ~ctest100()
    {
      ctx.state += MAGIC_DTOR100;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test100()
  {
    context ctx ( 3*SPECIAL_EXCEPTION100 + 1*MAGIC_DTOR100);  // 3 catches are expected + 1 dtor
    ctx.state = EH_OK;
    try
    {
      ctest100 ct100(ctx);
    }
    catch (int i)
    {
      if ( SPECIAL_EXCEPTION100 == i )
      {
        ctx.state += i;
      }
      else
      {
        ctx.state = UNEXPECTED_CATCH2;
      }
    }
    catch (...)
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

#if defined (_MSC_VER) && (_MSC_VER < 1310)
    //  ddk2600 x86 cl seems to generate a bit bugly unwinding code so let's just return a good context balance and see the actual ctx in debugger
    return 0;
#else
    return ctx.balance();
#endif  // (_MSC_VER < 1310)
  }

}  }  }


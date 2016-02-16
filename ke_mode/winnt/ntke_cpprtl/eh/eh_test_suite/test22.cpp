/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
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
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    UNEXPECTED_CATCH1         = -2,
    UNEXPECTED_CATCH2         = -3,
    UNEXPECTED_CATCH3         = -4,
    UNEXPECTED_CATCH4         = -5,
    SPECIAL_EXCEPTION22       = 22,
    MAGIC_DTOR22              = 1010,
  };
}


namespace
{

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
      ctx.state += MAGIC_DTOR22;
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
      : base00  ( c_ )
      , ctx     ( c_ )
    {
      exc_test et(c_);
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    ~base01()
    {
      ctx.state += MAGIC_DTOR22;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  struct ctest22
    : public base01
  {
    context& ctx;

    ctest22(context& c_) 
    try
      : base01  ( c_ )
      , ctx     ( c_ )
    {
      throw int(SPECIAL_EXCEPTION22);
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }
    catch (int i)
    {
      exc_test et(c_);
      if ( SPECIAL_EXCEPTION22 == i )
      {
        exc_test et(c_);
        c_.state += i;
      }
      else
      {
        c_.state = UNEXPECTED_CATCH4;
      }
    //  implicit 'throw;' is here
    }
    catch ( ... )
    {
      c_.state = UNEXPECTED_CATCH3;
    }

    ~ctest22()
    {
      ctx.state += MAGIC_DTOR22;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };

}


namespace cpprtl { namespace test { namespace eh
{

  int test22()
  {
    context ctx ( 2*SPECIAL_EXCEPTION22 + 2*MAGIC_DTOR22);  //  2 catches are expected + 2 dtor are expected
    ctx.state = EH_OK;
    try
    {
      ctest22 ct22(ctx);
    }
    catch (int i)
    {
      if ( SPECIAL_EXCEPTION22 == i )
      {
        ctx.state += i;
      }
      else
      {
        ctx.state = UNEXPECTED_CATCH2;
      }
    }
    catch ( ... )
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

#if defined ( _MSC_VER ) && ( _MSC_VER < 1310 )
    //  ddk2600 x86 cl seems to generate a bit buggy code so let's just return a good context balance and take a look at the actual ctx in a debugger
    return 0;
#else
    return ctx.balance();
#endif  //  _MSC_VER < 1310
  }

}  }  }


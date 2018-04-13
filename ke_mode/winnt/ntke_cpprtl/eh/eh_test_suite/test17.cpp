/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing an inherited class destruction after throwing constructor
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK                = 0
  , SPECIAL_EXCEPTION17  = 170
  , UNEXPECTED_CATCH1    = 171
  , UNEXPECTED_CATCH2    = 172
  , MAGIC_VALUE17        = 173
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
      ctx.state += MAGIC_VALUE17;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  class base01
    : public base00
  {
    context& ctx;
    base00 b00;

  public:
    base01(context& c_)
      : base00  ( c_ )
      , ctx     ( c_ )
      , b00     ( c_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    ~base01()
    {
      ctx.state += MAGIC_VALUE17;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  struct inh_test
    : public base01
  {
    context& ctx;
    base01 b01;

    inh_test(context& c_) 
      : base01  ( c_ )
      , ctx     ( c_ )
      , b01     ( c_ )
    {
      throw int(SPECIAL_EXCEPTION17);  // we do throw here and expecting '6' destructors invoked
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    ~inh_test()
    {
      ctx.state += MAGIC_VALUE17;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test17()
  {
    context ctx(MAGIC_VALUE17 * 6);
    ctx.state = EH_OK;
    try
    {
      inh_test inh_obj(ctx);
    }
    catch (int const& i)
    {
      if ( SPECIAL_EXCEPTION17 != i )
      {
        ctx.state = UNEXPECTED_CATCH2;
      }
    }
    catch (...)
    {
      ctx.state = UNEXPECTED_CATCH1;
    }

    return ctx.balance();
  }

}  }  }


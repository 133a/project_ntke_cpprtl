/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing an virtual inherited class destruction after throwing constructor
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK                = 0
  , SPECIAL_EXCEPTION18  = 180
  , UNEXPECTED_CATCH1    = 181
  , UNEXPECTED_CATCH2    = 182
  , MAGIC_CTOR           = 183
  , MAGIC_VBCTOR         = 184
  , MAGIC_DTOR           = 185
  , MAGIC_VBDTOR         = 186
  };


  class base00
  {
    context& ctx;

  public:
    explicit base00(context& c_)
      : ctx  ( c_ )
    {
      ++ctx.vctor_count;
      ++ctx.xtor_count;
      ctx.state += MAGIC_VBCTOR;
    }

    ~base00()
    {
      ctx.state += MAGIC_VBDTOR;
      ++ctx.vdtor_count;
      --ctx.xtor_count;
    }
  };


  class base10
    : public virtual base00
  {
    context& ctx;

  public:
    base10(context& c_)
      : base00  ( c_ )
      , ctx     ( c_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
      ctx.state += MAGIC_CTOR;
    }

    ~base10()
    {
      ctx.state += MAGIC_DTOR;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  class base11
    : public virtual base00
  {
    context& ctx;

  public:
    base11(context& c_)
      : base00  ( c_ )
      , ctx     ( c_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
      ctx.state += MAGIC_CTOR;
    }

    ~base11()
    {
      ctx.state += MAGIC_DTOR;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  struct cvtest18
    : public base10
    , public base11
  {
    context& ctx;

    cvtest18(context& c_) 
      : base00  ( c_ )
      , base10  ( c_ )
      , base11  ( c_ )
      , ctx     ( c_ )
    {
      throw int(SPECIAL_EXCEPTION18);  // we do throw here and expecting (2*MAGIC_CTOR + 1*MAGIC_VBCTOR + 2*MAGIC_DTOR + 1*MAGIC_VBDTOR)
      ++ctx.ctor_count;
      ++ctx.xtor_count;
      ctx.state += MAGIC_CTOR;
    }

    ~cvtest18()
    {
      ctx.state += MAGIC_DTOR;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test18()
  {
    context ctx(2*MAGIC_CTOR + 1*MAGIC_VBCTOR + 2*MAGIC_DTOR + 1*MAGIC_VBDTOR);
    ctx.state = EH_OK;
    try
    {
      cvtest18 cv18(ctx);
    }
    catch (int const& i)
    {
      if ( SPECIAL_EXCEPTION18 != i )
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


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing a try-catch block UDT with inheritance
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK              = 0,
    UNEXPECTED_CATCH1  = -2,
    UNEXPECTED_CATCH2  = -3,
    UNEXPECTED_CATCH3  = -4,
    MAGIC_VALUE15      = 8798,
  };


  class exc_base00
  {
    context& ctx;
    int i00;

  public:
    explicit exc_base00(context& c_)
      : ctx  ( c_ )
      , i00  ( 123 )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    exc_base00(exc_base00 const& src)
      : ctx  ( src.ctx )
      , i00  ( src.i00 )
    {
      ++ctx.cctor_count;
      ++ctx.xtor_count;
    }

    virtual ~exc_base00()
    {
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  class exc_base01
    : public exc_base00
  {
    context& ctx;
    int i01;

  public:
    exc_base01(context& c_)
      : exc_base00  ( c_ )
      , ctx         ( c_ )
      , i01         ( 321 )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    exc_base01(exc_base01 const& src)
      : exc_base00  ( src )
      , ctx         ( src.ctx )
      , i01         ( src.i01 )
    {
      ++ctx.cctor_count;
      ++ctx.xtor_count;
    }

    virtual ~exc_base01()
    {
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };


  struct inh_test
    : public exc_base01
  {
    context& ctx;
    unsigned i;

    inh_test(context& c_, int const& i_) 
      : exc_base01  ( c_ )
      , ctx         ( c_ )
      , i           ( i_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    inh_test(inh_test const& src)
      : exc_base01  ( src )
      , ctx         ( src.ctx )
      , i           ( src.i )
    {
      ++ctx.cctor_count;
      ++ctx.xtor_count;
    }

    ~inh_test()
    {
      i = 0;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test15()
  {
    context ctx(MAGIC_VALUE15 * 3);  // the state is incremented every time the right exception is caught - and this is expected 3 times
    ctx.state = EH_OK;
    try
    {
      try 
      {
        throw inh_test(ctx, MAGIC_VALUE15);
      }
      catch (exc_base00& e)
      {
        // we can do the cast 'cos the object is caught by ref and the derived object is certainly known to be in place
        ctx.state += (static_cast<inh_test const&>(e)).i;
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_CATCH1;
        throw;
      }

      try 
      {
        throw inh_test(ctx, MAGIC_VALUE15);
      }
      catch (exc_base01& e)
      {
        ctx.state += (static_cast<inh_test const&>(e)).i;
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_CATCH2;
        throw;
      }

      try 
      {
        throw inh_test(ctx, MAGIC_VALUE15);
      }
      catch (inh_test& ex)
      {
        ctx.state += ex.i;
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_CATCH3;
        throw;
      }
    }
    catch (...)
    {
    }

    return ctx.balance();
  }

}  }  }


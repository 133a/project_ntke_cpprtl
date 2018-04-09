/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing a try-catch block in another catch scope for UDT with virtual inheritance
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
    MAGIC_VALUE14      = 65897,
  };


  struct vtest
    : virtual public eh_test
  {
    context& ctx;
    unsigned i;

    vtest(context& c_, int const& i_)
      : eh_test  ( c_ )
      , ctx      ( c_ )
      , i        ( i_ )
    {
      ++ctx.vctor_count;
      ++ctx.xtor_count;
    }

    vtest(vtest const& src)
      : eh_test  ( src )
      , ctx      ( src.ctx )
      , i        ( src.i )
    {
      ++ctx.vcctor_count;
      ++ctx.xtor_count;
    }

    ~vtest()
    {
      i = 0;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test14()
  {
    context ctx(MAGIC_VALUE14);
    {
      try 
      {
        throw vtest(ctx, MAGIC_VALUE14);
      }
      catch (vtest& ex)
      {
        if (MAGIC_VALUE14 == ex.i)
        {
          try
          {
            throw;
          }
          catch (vtest/*&*/ ex)
          {
            ctx.state = ex.i;
          }
          catch (...)
          {
            ctx.state = UNEXPECTED_CATCH2;
          }
        }
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_CATCH1;
      }
    }
    return ctx.balance();
  }

}  }  }


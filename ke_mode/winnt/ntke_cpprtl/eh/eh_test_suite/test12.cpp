/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing UDT exception object propagation to the outer scope
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK              = 0
  , MAGIC_VALUE12      = 120
  , UNEXPECTED_CATCH1  = 121
  , UNEXPECTED_CATCH2  = 122
  , UNEXPECTED_CATCH3  = 123
  };


  void ff_throw2(context& ctx)
  {
    try
    {
      throw ;  // !
    }
    catch (int&)
    {
      ctx.state = UNEXPECTED_CATCH3;
    }
  }


  void ff_catch_by_val(context& ctx)
  {
    try
    {
      throw eh_test(ctx, MAGIC_VALUE12);  // this magic value is expected to be catched in test12()
    }
    catch (eh_test ex)
    {
      try 
      {
        ff_throw2(ctx);
      }
      catch (int)
      {
        ctx.state = UNEXPECTED_CATCH2;
      }
    }
  }


  void ff_catch_by_ref(context& ctx)
  {
    try
    {
      throw eh_test(ctx, MAGIC_VALUE12);  // this magic value is expected to be catched in test12()
    }
    catch (eh_test&)
    {
      try 
      {
        ff_throw2(ctx);
      }
      catch (int)
      {
        ctx.state = UNEXPECTED_CATCH1;
      }
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test12()
  {
    context ctx(2 * MAGIC_VALUE12);  // 2 catches are expected - by val and by ref
    ctx.state = EH_OK;
    {
      try 
      {
        ff_catch_by_val(ctx);
      }
      catch (eh_test ex)
      {
        ctx.state += ex.i;
      }

      try 
      {
        ff_catch_by_ref(ctx);
      }
      catch (eh_test& ex)
      {
        ctx.state += ex.i;
      }
    }
    return ctx.balance();
  }

}  }  }


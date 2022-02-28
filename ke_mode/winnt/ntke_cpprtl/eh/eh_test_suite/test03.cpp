//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// stack unwind/cleanup tests
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      DTOR  = 3
    , THROW = 5
    };

    typedef xtor_counter<0> UDT;

    struct test
      : protected UDT
    {
      explicit test(context& c) 
        : UDT ( c )
      {
        ctx.ctor();
      }

      test(test const& src)
        : UDT ( src )
      {
        ctx.cctor();
      }

      ~test()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }
    };
  }  // namespace

//==============
// frame cleanup
//--------------
  bool test_0301()
  {
    context ctx(THROW + 3*DTOR);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        test t1(ctx);
        test t2(ctx);
        test t3(ctx);
        throw int(THROW);
        test t4(ctx);
        test t5(ctx);
      }
      catch (int exc)
      {
        ctx.state += exc;
      }
    }
    return ctx.ok();
  }

//=============
// stack unwind
//-------------
  namespace
  {
    enum { DEPTH = 10 };

    void f_02(context& ctx, unsigned depth)
    {
      test t1(ctx);
      test t2(ctx);
      test t3(ctx);
      if ( !--depth )
      {
        test t1(ctx);
        test t2(ctx);
        test t3(ctx);
        throw int(THROW);
      }
      f_02(ctx, depth);
    }
  }  // namespace

  bool test_0302()
  {
    context ctx(THROW + 3*DTOR*DEPTH + 3*DTOR);
    try
    {
      f_02(ctx, DEPTH);
    }
    catch (int exc)
    {
      ctx.state += exc;
    }
    return ctx.ok();
  }

//========================
// return from catch-block
//------------------------
  namespace
  {
    enum
    {
      CATCH      = 321
    , UNEXPECTED = 5
    };

    int f_03(context& ctx)
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        test t1(ctx);
        test t2(ctx);
        test t3(ctx);
        throw t1;
      }
      catch (test exc)
      {
        test t1 = exc;
        test t2 = t1;
        test t3 = t2;
        return CATCH;
      }
      return UNEXPECTED;
    }
  }  // namespace

  bool test_0303()
  {
    context ctx(CATCH + 6*DTOR + DTOR + DTOR);
    ctx.state += f_03(ctx);
    return ctx.ok();
  }

//========================
// goto out of catch-block
//------------------------
  namespace { enum { COUNTER = 17 }; }

  bool test_0304()
  {
    context ctx(COUNTER + 12*DTOR*COUNTER);
    for ( unsigned idx = 0; idx < COUNTER ; ++idx )
    {
      test t1(ctx);
      test t2(ctx);
      test t3(ctx);
      try
      {
        test t1(ctx);
        test t2(ctx);
        test t3(ctx);
        throw int(THROW);
      }
      catch (int const exc)
      {   
        test t1(ctx);
        test t2(ctx);
        test t3(ctx);
        if ( THROW == exc )
        {
          test t1(ctx);
          test t2(ctx);
          test t3(ctx);
          ++ctx.state;
          continue;
        }
        ctx.state = UNEXPECTED;
        break;
      }
      ctx.state = UNEXPECTED*2;
      break;
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

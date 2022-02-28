//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// MT-UNsafe, run once
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
//=================================================
// function scope static object throwing ctor/cctor
//-------------------------------------------------
  namespace
  {
    enum { THROW = 3 };

    struct test
    {
      static unsigned idx;
      context& ctx;

      explicit test(context& c)
        : ctx ( c )
      {
        if ( idx < THROW )
        {
          throw int(THROW);
        }
        ctx.ctor();
      }
      test(test& src)
        : ctx ( src.ctx )
      {
        if ( idx < 2*THROW )
        {
          throw int(2*THROW);
        }
        ctx.cctor();
      }
      // ~test() keep trivial to avoid atexit()
    };
    unsigned test::idx = 0;
  }  // namespace

  bool test_5201()
  {
    context ctx(THROW*THROW + THROW*2*THROW);
    for ( test::idx = 0; test::idx < 3*THROW ; ++test::idx )
    {
      try
      {
        static test t1(ctx);
        static test t2(t1);
      }
      catch (int const exc)
      {
        ctx.state += exc;
      }
    }
    ctx.dtor();  // simulate test::~test()
    ctx.dtor();  // ^
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

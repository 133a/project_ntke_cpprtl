//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// nested exceptions in exception object cctor/dtor or unwind action scope
// NOTE these exceptions are dispatched on top of the stack_walk() unwinding loop
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      THROW      = 29
    , CATCH      = 31
    , UNEXPECTED = 37
    };
    typedef xtor_counter<0> UDT;
    typedef xtor_counter<THROW> EXC;
  }

  namespace
  {
    int f_00(context& ctx)
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        try
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          throw EXC(ctx);
        }
        catch (EXC const& exc)
        {
          ctx.state += exc.val;
        }
      }
      catch (...)
      {
        return UNEXPECTED;
      }
      return THROW;
    }

    struct test
    {
      context& ctx;

      explicit test(context& c)
        : ctx ( c )
      {
        ctx.ctor();
      }
      test(test const& src)
        : ctx ( src.ctx )
      {
        ctx.state += f_00(ctx);
        ctx.cctor();
      }
      ~test()
      {
        ctx.state += f_00(ctx);
        ctx.dtor();
      }
    };
  }  // namespace

//===============================================================
// exception handling in scope of the exception object cctor/dtor
//---------------------------------------------------------------
  bool test_0901()
  {
    context ctx(2*THROW + 4*THROW + CATCH);
    try
    {
      throw test(ctx);
    }
    catch (test exc)
    {
      ctx.state += CATCH;
    }
    return ctx.ok();
  }

//=====================================
// exception handling in unwind actions
//-------------------------------------
  bool test_0902()
  {
    context ctx(4*THROW + THROW);
    try
    {
      test t(ctx);
      try
      {
        test t(ctx);
        throw EXC(ctx);
      }
      catch (UDT&)
      {
        ctx.state += UNEXPECTED;
      }
    }
    catch (EXC const& exc)
    {
      ctx.state += exc.val;
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

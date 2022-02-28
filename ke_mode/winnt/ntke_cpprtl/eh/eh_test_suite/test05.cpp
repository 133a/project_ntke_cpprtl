//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// rethrow/nested throw tests
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      THROW   = 73
    , E11_VAL = 11
    , E17_VAL = 17
    };
    typedef xtor_counter<0> UDT;
    typedef xtor_counter<THROW> EXC;
    typedef xtor_counter<E11_VAL> E11;
    typedef xtor_counter<E17_VAL> E17;
  }

//=============================
// BIT rethrow to nesting scope
//-----------------------------
  namespace
  {
    void f_01(context& ctx)
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        throw int(THROW);
      }
      catch (int const exc)
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        ctx.state += exc;
        throw;
      }
    }
  }  // namespace

  bool test_0501()
  {
    context ctx(2*THROW);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        f_01(ctx);
      }
      catch (int const exc)
      {
        ctx.state += exc;
      }
    }
    return ctx.ok();
  }

//=============================
// UDT rethrow to nesting scope
//-----------------------------
  namespace
  {
    void f_02(context& ctx)
    {
      try
      {
        throw EXC(ctx);
      }
      catch (EXC exc)
      {
        ctx.state += exc.val;
        throw;
      }
    }
  }  // namespace

  bool test_0502()
  {
    context ctx(2*THROW);
    {
      try
      {
        f_02(ctx);
      }
      catch (EXC const exc)
      {
        ctx.state += exc.val;
      }
    }
    return ctx.ok();
  }

  namespace
  {
    void f_03(context& ctx)
    {
      try
      {
        throw EXC(ctx);
      }
      catch (EXC& exc)
      {
        ctx.state += exc.val;
        throw;
      }
    }
  }  // namespace

  bool test_0503()
  {
    context ctx(2*THROW);
    {
      try
      {
        f_03(ctx);
      }
      catch (EXC const& exc)
      {
        ctx.state += exc.val;
      }
    }
    return ctx.ok();
  }

//============================
// BIT rethrow to nested scope
//----------------------------
  namespace
  {
    void f_04(context& ctx)
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        throw;
      }
      catch (int const exc)
      {
        ctx.state = exc;
      }
    }
  }  // namespace

  bool test_0504()
  {
    context ctx(THROW);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        throw int(THROW);
      }
      catch (int)
      {
        f_04(ctx);
      }
    }
    return ctx.ok();
  }

//============================
// UDT rethrow to nested scope
//----------------------------
  namespace
  {
    void f_05(context& ctx)
    {
      try
      {
        throw;
      }
      catch (EXC exc)
      {
        ctx.state += exc.val;
      }
    }
  }  // namespace

  bool test_0505()
  {
    context ctx(2*THROW);
    {
      try
      {
        throw EXC(ctx);
      }
      catch (EXC const exc)
      {
        ctx.state += exc.val;
        f_05(ctx);
      }
    }
    return ctx.ok();
  }

  namespace
  {
    void f_06(context& ctx)
    {
      try
      {
        throw;
      }
      catch (EXC const& exc)
      {
        ctx.state += exc.val;
      }
    }
  }  // namespace

  bool test_0506()
  {
    context ctx(2*THROW);
    {
      try
      {
        throw EXC(ctx);
      }
      catch (EXC& exc)
      {
        ctx.state += exc.val;
        f_06(ctx);
      }
    }
    return ctx.ok();
  }

//=============================
// UDT rethrow to outward scope
//-----------------------------
  namespace
  {
    void g_07(context& ctx)
    {
      try
      {
        throw;
      }
      catch (...)
      {
        ctx.state += THROW;
        throw;
      }
    }

    void f_07_val(context& ctx)
    {
      try
      {
        throw EXC(ctx);
      }
      catch (EXC exc)
      {
        ctx.state += exc.val;
        g_07(ctx);
      }
    }

    void f_07_ref(context& ctx)
    {
      try
      {
        throw EXC(ctx);
      }
      catch (EXC& exc)
      {
        ctx.state += exc.val;
        g_07(ctx);
      }
    }
  }  // namespace

  bool test_0507()
  {
    context ctx(3*THROW + 3*THROW);
    try 
    {
      f_07_val(ctx);
    }
    catch (EXC const exc)
    {
      ctx.state += exc.val;
    }
    try 
    {
      f_07_ref(ctx);
    }
    catch (EXC const& exc)
    {
      ctx.state += exc.val;
    }
    return ctx.ok();
  }

//=================
// UDT nested throw
//-----------------
  namespace
  {
    void f_08(context& ctx)
    {
      try
      {
        throw E11(ctx);
      }
      catch (E11 exc)
      {
        ctx.state += exc.val;
        throw E17(ctx);
      }
    }
  }  // namespace

  bool test_0508()
  {
    context ctx(E11_VAL + E17_VAL);
    {
      try
      {
        f_08(ctx);
      }
      catch (E17 exc)
      {
        ctx.state += exc.val;
      }
    }
    return ctx.ok();
  }

  namespace
  {
    void f_09(context& ctx)
    {
      try
      {
        throw E17(ctx);
      }
      catch (E17& exc)
      {
        ctx.state += exc.val;
      }
    }
  }  // namespace

  bool test_0509()
  {
    context ctx(E11_VAL + E17_VAL);
    {
      try
      {
        throw E11(ctx);
      }
      catch (E11& exc)
      {
        ctx.state += exc.val;
        f_09(ctx);
      }
    }
    return ctx.ok();
  }

  namespace
  {
    void f_10(context& ctx)
    {
      try
      {
        throw;
      }
      catch (...)
      {
        ctx.state += THROW;
        throw E17(ctx);
      }
    }

    void f_10_ref(context& ctx)
    {
      try
      {
        throw E11(ctx);
      }
      catch (E11& exc)
      {
        ctx.state += exc.val;
        f_10(ctx);
      }
    }
  }  // namespace

  bool test_0510()
  {
    context ctx(THROW + E11_VAL + E17_VAL);
    try 
    {
      f_10_ref(ctx);
    }
    catch (E17 const& exc)
    {
      ctx.state += exc.val;
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

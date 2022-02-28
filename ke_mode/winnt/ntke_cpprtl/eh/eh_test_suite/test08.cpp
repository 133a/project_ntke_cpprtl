//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// function-try-blocks tests
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      THROW      = 17
    , UNEXPECTED = 19
    , DTOR       = 23
    };

    typedef xtor_counter<0> UDT;
    typedef xtor_counter<THROW> EXC;
  }

//===================
// function-try-block
//-------------------
  namespace
  {
    int f_01(context& ctx)
    try
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      throw EXC(ctx);
      return UNEXPECTED;
    }
    catch (EXC const exc)
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      ctx.state += exc.val;
      return exc.val;
    }
  }  // namespace

  bool test_0801()
  {
    context ctx(2*THROW);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        ctx.state += f_01(ctx);
      }
      catch (...)
      {
        ctx.state = UNEXPECTED;
      }
    }
    return ctx.ok();
  }


//========================
// ctor function-try-block
//------------------------
  namespace
  {
    template <unsigned N>
    class test;
    
    template <>
    class test<0>
    {
      context& ctx;

    public:
      test(context& c, int const do_throw = -1)
        : ctx ( c )
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        if ( 0 == do_throw )
        {
          throw EXC(ctx);
        }
        ctx.ctor();
      }

      ~test()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }
    };

    template <unsigned N>
    class test : public test<N-1>
    {
      typedef test<N-1> base_type;
      context&  ctx;
      base_type val;

    public:
      test(context& c, int const do_throw = -1)
      try
        : base_type ( c, do_throw-1 ) 
        , ctx       ( c )
        , val       ( c )
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        if ( 0 == do_throw )
        {
          throw EXC(ctx);
        }
        ctx.ctor();
      }
      catch (EXC const& exc)
      {
        UDT udt1(c);
        UDT udt2(c);
        c.state += exc.val;
      #if defined (_MSC_VER) && (_MSC_VER < 1310)
        throw;  // looks like {msvc2002/ddk2600}-x86 doesn't emit the implicit `throw;'
      #else
      // implicit `throw;'
      #endif
      }

      ~test()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }
    };

    enum { BASES = 3 };
    typedef test<BASES> test_type;
  }  // namespace

  bool test_0802()
  {
    context ctx
    (
      THROW + (BASES)*THROW   + 0*DTOR
    + THROW + (BASES)*THROW   + 2*DTOR
    + THROW + (BASES-1)*THROW + 6*DTOR
    + THROW + (BASES-2)*THROW + 14*DTOR
    );
    for ( int throw_at = BASES; throw_at >= 0; --throw_at )
    {                        
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        test_type t(ctx, throw_at);
        UDT udt3(ctx);
        UDT udt4(ctx);
      }
      catch (EXC const& exc)
      {
        ctx.state += exc.val;
      }
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

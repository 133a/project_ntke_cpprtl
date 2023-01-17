//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// function-try-block tests
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


//========================
// dtor function-try-block
//------------------------
  namespace
  {
    class dtor_ftb
    {
      context& ctx;
      UDT u;

    public:
      explicit dtor_ftb(context& c)
        : ctx ( c )
        , u   ( c )
      {
        ctx.ctor();
      }

      ~dtor_ftb() throw(...)
      try
      {
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          ctx.state += DTOR;
          ctx.dtor();
        }
      #if defined (_MSC_VER) && (_MSC_VER < 1310)  // looks like {msvc2002/ddk2600}-x86 doesn't emit member destructors list for dtor-ftb
        {
          u.~UDT();  // simulate members destruction, the frame state tracking isn't necessary here 
        }
      #endif
      }
      catch (...)
      {
      #if defined (_MSC_VER) && (_MSC_VER < 1310)
        throw;
      #else
      // implicit `throw;'
      #endif
      }
    };
  }  // namespace

  bool test_0803()
  {
    context ctx(DTOR);
    {
      UDT udt(ctx);
      try
      {
        UDT udt1(ctx);
        dtor_ftb test(ctx);
        UDT udt2(ctx);
      }
      catch (...)
      {
        ctx.state = UNEXPECTED;
      }
    }
    return ctx.ok();
  }


  namespace
  {
    class dtor_ftb_throw
    {
      context& ctx;
      UDT u;

    public:
      explicit dtor_ftb_throw(context& c)
        : ctx ( c )
        , u   ( c )
      {
        ctx.ctor();
      }

      ~dtor_ftb_throw() throw(...)
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        ctx.state += DTOR;
        ctx.dtor();
        throw EXC(ctx);
      }
      catch (EXC const& exc)
      {
        exc.ctx.state += exc.val;
      #if defined (_MSC_VER) && (_MSC_VER < 1310)
        throw;
      #else
      // implicit `throw;'
      #endif
      }
    };
  }  // namespace

  bool test_0804()
  {
    context ctx(DTOR + THROW + THROW);
    {
      UDT udt(ctx);
      try
      {
        UDT udt1(ctx);
        dtor_ftb_throw test(ctx);
        UDT udt2(ctx);
      }
      catch (EXC const& exc)
      {
        ctx.state += exc.val;
      }
      catch (...)
      {
        ctx.state = UNEXPECTED;
      }
    }
    return ctx.ok();
  }


  namespace
  {
    class dtor_ftb_suppress_rethrow
    {
      context& ctx;
      UDT u1;
      UDT u2;

    public:
      explicit dtor_ftb_suppress_rethrow(context& c)
        : ctx ( c )
        , u1  ( c )
        , u2  ( c )
      {
        ctx.ctor();
      }

      ~dtor_ftb_suppress_rethrow() throw(...)
      try
      {
        UDT udt(ctx);
        ctx.state += DTOR;
        ctx.dtor();
        throw EXC(ctx);
      }
      catch (EXC const& exc)
      {
        exc.ctx.state += exc.val;
        return;  // suppress the implicit `throw;' ({msvc2002/ddk2600}-x86 doesn't emit it anyway)
      }
    };
  }  // namespace

  bool test_0805()
  {
    context ctx(DTOR + THROW);
    {
      UDT udt(ctx);
      try
      {
        UDT udt1(ctx);
        dtor_ftb_suppress_rethrow test(ctx);
        UDT udt2(ctx);
      }
      catch (...)
      {
        ctx.state = UNEXPECTED;
      }
    }
    return ctx.ok();
  }


  namespace
  {
    class dtor_ftb_suppress_rethrow_2
    {
      context& ctx;
      UDT u1;
      dtor_ftb_throw d;
      UDT u2;

    public:
      explicit dtor_ftb_suppress_rethrow_2(context& c)
        : ctx ( c )
        , u1  ( c )
        , d   ( c )
        , u2  ( c )
      {
        ctx.ctor();
      }

      ~dtor_ftb_suppress_rethrow_2()
      try
      {
        {
          UDT udt(ctx);
          ctx.state += DTOR;
          ctx.dtor();
        }
      #if defined (_MSC_VER) && (_MSC_VER < 1310)
        {  // simulate members destruction
          u2.~UDT();             
        #ifdef _M_IX86
          __asm mov dword ptr [ebp-4] , 1  // pull the frame state down to `d' scope
        #else
          #error check frame state
        #endif
          d.~dtor_ftb_throw();
          u1.~UDT();
        }
      #endif  // _MSC_VER < 1310
      }
      catch (EXC const& exc)
      {
        exc.ctx.state += exc.val;
        return;  // suppress the implicit `throw;'
      }
    };
  }  // namespace

  bool test_0806()
  {
    context ctx(2*DTOR + 2*THROW);
    {
      UDT udt(ctx);
      try
      {
        UDT udt1(ctx);
        dtor_ftb_suppress_rethrow_2 test(ctx);
        UDT udt2(ctx);
      }
      catch (...)
      {
        ctx.state = UNEXPECTED;
      }
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

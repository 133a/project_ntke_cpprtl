//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// inheritance tests
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      VALUE = 5
    , THROW = 11
    , DTOR  = 17
    };
  }

  namespace
  {
    template <unsigned N>
    class test;
    
    template <>
    class test<0>
    {
      context& ctx;
      unsigned val;

    public:
      explicit test(context& c)
        : ctx ( c )
        , val ( 0 )
      {
        ctx.ctor();
      }

      test(test const& src)
        : ctx ( src.ctx )
        , val ( src.val )
      {
        ctx.cctor();
      }

      virtual ~test()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }

      void set(unsigned const v)
      {
        val = v;
      }

      virtual unsigned value() const
      {
        return val;
      }
    };

    template <unsigned N>
    class test : public test<N-1>
    {
      typedef test<N-1> base_type;
      context& ctx;

    public:
      explicit test(context& c)
        : base_type ( c ) 
        , ctx       ( c )
      {
        ctx.ctor();
      }

      test(test const& src)
        : base_type ( src ) 
        , ctx       ( src.ctx )
      {
        ctx.cctor();
      }

      ~test()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }

      virtual unsigned value() const
      {
        return (N+1) * test<0>::value();
      }
    };
  }  // namespace

  namespace
  {
    template <unsigned N>
    class base : public virtual test<0>
    {
      context& ctx;

    public:
      explicit base(context& c)
        : test<0>  ( c ) 
        , ctx      ( c )
      {
        ctx.ctor();
      }

      base(base const& src)
        : test<0> ( src ) 
        , ctx     ( src.ctx )
      {
        ctx.cctor();
      }

      ~base()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }

      virtual unsigned value() const
      {
        return (N+1) * test<0>::value();
      }
    };

    class vbtest
      : public base<1>
      , public base<2>
    {
      context& ctx;

    public:
      explicit vbtest(context& c)
        : test<0> ( c ) 
        , base<1> ( c )
        , base<2> ( c )
        , ctx     ( c )
      {
        ctx.ctor();
      }

      vbtest(vbtest const& src)
        : test<0> ( src ) 
        , base<1> ( src )
        , base<2> ( src )
        , ctx     ( src.ctx )
      {
        ctx.cctor();
      }

      ~vbtest()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }

      virtual unsigned value() const
      {
        return 4 * test<0>::value();
      }
    };
  }  // namespace

//==============================
// catch by base class reference
//------------------------------
  bool test_0401()
  {
    context ctx(4*4*VALUE + 5*4*DTOR);
    {
      test<3> t(ctx);
      t.set(VALUE);
      try 
      {
        throw t;
      }
      catch (test<0>& exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (test<1>& exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (test<2>& exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (test<3>& exc)
      {
        ctx.state += exc.value();
      }
    }
    return ctx.ok();
  }

  bool test_0402()
  {
    context ctx(4*4*VALUE + 5*4*DTOR);
    {
      vbtest t(ctx);
      t.set(VALUE);
      try 
      {
        throw t;
      }
      catch (test<0>& exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (base<1>& exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (base<2>& exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (vbtest& exc)
      {
        ctx.state += exc.value();
      }
    }
    return ctx.ok();
  }

//============================
// catch by base class pointer
//----------------------------
  bool test_0403()
  {
    context ctx(4*4*VALUE + 4*DTOR);
    {
      test<3> t(ctx);
      t.set(VALUE);
      try 
      {
        throw &t;
      }
      catch (test<0>* exc)
      {
        ctx.state += exc->value();
      }
      try 
      {
        throw &t;
      }
      catch (test<1>* exc)
      {
        ctx.state += exc->value();
      }
      try 
      {
        throw &t;
      }
      catch (test<2>* exc)
      {
        ctx.state += exc->value();
      }
      try 
      {
        throw &t;
      }
      catch (test<3>* exc)
      {
        ctx.state += exc->value();
      }
    }
    return ctx.ok();
  }

  bool test_0404()
  {
    context ctx(4*4*VALUE + 4*DTOR);
    {
      vbtest t(ctx);
      t.set(VALUE);
      try 
      {
        throw &t;
      }
      catch (test<0>* exc)
      {
        ctx.state += exc->value();
      }
      try 
      {
        throw &t;
      }
      catch (base<1>* exc)
      {
        ctx.state += exc->value();
      }
      try 
      {
        throw &t;
      }
      catch (base<2>* exc)
      {
        ctx.state += exc->value();
      }
      try 
      {
        throw &t;
      }
      catch (vbtest* exc)
      {
        ctx.state += exc->value();
      }
    }
    return ctx.ok();
  }

//==================================
// catch by exception object slicing
//----------------------------------
  bool test_0405()
  {
    context ctx
    (
      VALUE + 2*VALUE + 3*VALUE + 4*VALUE
    + 5*4*DTOR + DTOR + 2*DTOR + 3*DTOR + 4*DTOR
    );
    {
      test<3> t(ctx);
      t.set(VALUE);
      try 
      {
        throw t;
      }
      catch (test<0> exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (test<1> exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (test<2> exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (test<3> exc)
      {
        ctx.state += exc.value();
      }
    }
    return ctx.ok();
  }

  bool test_0406()
  {
    context ctx
    (
      VALUE + 2*VALUE + 3*VALUE + 4*VALUE
    + 5*4*DTOR + DTOR + 2*DTOR + 2*DTOR + 4*DTOR
    );
    {
      vbtest t(ctx);
      t.set(VALUE);
      try 
      {
        throw t;
      }
      catch (test<0> exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (base<1> exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (base<2> exc)
      {
        ctx.state += exc.value();
      }
      try 
      {
        throw t;
      }
      catch (vbtest exc)
      {
        ctx.state += exc.value();
      }
    }
    return ctx.ok();
  }

//=========================
// cleanup after ctor threw
//-------------------------
  namespace
  {
    class test_throw : public test<3>
    {
      context& ctx;
      test<3>  tst;

    public:
      explicit test_throw(context& c)
        : test<3> ( c )
        , ctx     ( c )
        , tst     ( c )
      {
        throw int(THROW);
        ctx.ctor();
      }

      test_throw(test_throw const& src)
        : test<3> ( src ) 
        , ctx     ( src.ctx )
        , tst     ( src.tst )
      {
        ctx.cctor();
      }

      ~test_throw()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }
    };
  }  // namespace

  bool test_0407()
  {
    context ctx(THROW + 2*4*DTOR);
    try
    {
      test_throw t(ctx);
    }
    catch (int const exc)
    {
      ctx.state += exc;
    }
    return ctx.ok();
  }

//============================
// cleanup after ctor_vb threw
//----------------------------
  namespace
  {
    class vbtest_throw : public vbtest
    {
      context& ctx;
      vbtest   tst;

    public:
      explicit vbtest_throw(context& c)
        : test<0> ( c )
        , vbtest  ( c )
        , ctx     ( c )
        , tst     ( c )
      {
        throw int(THROW);
        ctx.ctor();
      }

      vbtest_throw(vbtest_throw const& src)
        : test<0> ( src )
        , vbtest  ( src ) 
        , ctx     ( src.ctx )
        , tst     ( src.tst )
      {
        ctx.cctor();
      }

      ~vbtest_throw()
      {
        ctx.state += DTOR;
        ctx.dtor();
      }
    };
  }  // namespace

  bool test_0408()
  {
    context ctx(THROW + 2*4*DTOR);
    try
    {
      vbtest_throw t(ctx);
    }
    catch (int const exc)
    {
      ctx.state += exc;
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

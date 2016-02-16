/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef CONTEXT_H_
#define CONTEXT_H_


namespace
{

  struct context
  {
    int const state_expected;
    int  state;
    int  flags;

    int  ctor_count;
    int  cctor_count;
    int  dtor_count;
    int  vctor_count;
    int  vcctor_count;
    int  vdtor_count;
    int  xtor_count;

    // ~context() keep it trivial for using in seh-frames

    explicit context(int expected)
      : state_expected  ( expected )
      , state           ( -1 )
      , flags           ( 0 )
      , ctor_count      ( 0 )
      , cctor_count     ( 0 )
      , dtor_count      ( 0 )
      , vctor_count     ( 0 )
      , vcctor_count    ( 0 )
      , vdtor_count     ( 0 )
      , xtor_count      ( 0 )
    {
    }

    int xtor_balance() const
    {
      return  xtor_count + ( ctor_count + cctor_count + vctor_count + vcctor_count - dtor_count - vdtor_count );
    }

    int state_balance() const
    {
      return state_expected - state;
    }

    int balance() const
    {
      return state_balance() | xtor_balance();
    }

  private:  //  context is intended to be the single instance in a test
    context(context const&);
    context& operator=(context&);
  };


  struct exc_test
  {
    context& ctx;
    unsigned i;

    explicit exc_test(context& c_, int const& i_ = 0)
      : ctx  ( c_ )
      , i    ( i_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    exc_test(exc_test const& src)
      : ctx  ( src.ctx )
      , i    ( src.i )
    {
      ++ctx.cctor_count;
      ++ctx.xtor_count;
    }

    ~exc_test()
    {
      i = 0;
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }

  };

}

#endif // include guard


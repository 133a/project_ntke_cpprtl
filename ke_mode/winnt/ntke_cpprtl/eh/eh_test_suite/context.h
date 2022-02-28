//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef CONTEXT_H_
#define CONTEXT_H_


namespace cpprtl { namespace eh { namespace test
{
  class context
  {
    // ~context() keep trivial

    int ctor_count;
    int cctor_count;
    int dtor_count;
    int xtor_count;
    int const expected;
  public:
    int state;

    explicit context(int const expected)
      : ctor_count  ( 0 )
      , cctor_count ( 0 )
      , dtor_count  ( 0 )
      , xtor_count  ( 0 )
      , expected    ( expected )
      , state       ( 0 )
    {}

    void ctor()
    {
      ++ctor_count;
      ++xtor_count;
    }

    void cctor()
    {
      ++cctor_count;
      ++xtor_count;
    }

    void dtor()
    {
      ++dtor_count;
      --xtor_count;
    }

    bool ok() const
    {
      return (state - expected == 0) &&
             (xtor_count + (ctor_count + cctor_count - dtor_count) == 0);
    }

  private:  // context is intended to be the single instance in a test
    context(context const&);
    context& operator=(context&);
  };


  template <int I>
  struct xtor_counter
  {
    context& ctx;
    int volatile val;

    explicit xtor_counter(context& c, int const i = I)
      : ctx ( c )
      , val ( i )
    {
      ctx.ctor();
    }

    xtor_counter(xtor_counter const& src)
      : ctx ( src.ctx )
      , val ( src.val )
    {
      ctx.cctor();
    }

    ~xtor_counter()
    {
      val = 0;
      ctx.dtor();
    }
  };

}}}  // namespace cpprtl::eh::test


#endif  // include guard

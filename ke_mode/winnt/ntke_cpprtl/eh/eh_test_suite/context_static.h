//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef CONTEXT_STATIC_H_
#define CONTEXT_STATIC_H_


namespace cpprtl { namespace eh { namespace test
{
  struct context
  {
    static int ctor_count;
    static int cctor_count;
    static int dtor_count;
    static int xtor_count;
    static int expected;
    static int state;

    static void init(int const st)
    {
      ctor_count   = 0;
      cctor_count  = 0;
      dtor_count   = 0;
      xtor_count   = 0;
      expected     = st;
      state        = 0;
    }

    static void ctor()
    {
      ++ctor_count;
      ++xtor_count;
    }

    static void cctor()
    {
      ++cctor_count;
      ++xtor_count;
    }

    static void dtor()
    {
      ++dtor_count;
      --xtor_count;
    }

    static bool ok()
    {
      return (state - expected == 0) &&
             (xtor_count + (ctor_count + cctor_count - dtor_count) == 0);
    }
  private:
    context();
  };

}}}  // namespace cpprtl::eh::test


#endif  // include guard

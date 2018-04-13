/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef CONTEXT_STATIC_H_
#define CONTEXT_STATIC_H_


namespace
{
  enum
  {
    STATE_FACTOR  = 10000
  , XTOR_FACTOR   = 1
  };


  struct context
  {
    static int ctor_count;
    static int cctor_count;
    static int dtor_count;
    static int vctor_count;
    static int vcctor_count;
    static int vdtor_count;
    static int xtor_count;

    static void init()
    {
      ctor_count   = 0;
      cctor_count  = 0;
      dtor_count   = 0;
      vctor_count  = 0;
      vcctor_count = 0;
      vdtor_count  = 0;
      xtor_count   = 0;
    }

    static int balance(int const& state)
    {
      return
      (
        state * STATE_FACTOR
      +
        (xtor_count + (ctor_count + cctor_count + vctor_count + vcctor_count - dtor_count - vdtor_count)) * XTOR_FACTOR
      );
    }
  private:
    context() {}
  };
  int context::ctor_count    = 0;
  int context::cctor_count   = 0;
  int context::dtor_count    = 0;
  int context::vctor_count   = 0;
  int context::vcctor_count  = 0;
  int context::vdtor_count   = 0;
  int context::xtor_count    = 0;

}  // namespace


#endif  // include guard



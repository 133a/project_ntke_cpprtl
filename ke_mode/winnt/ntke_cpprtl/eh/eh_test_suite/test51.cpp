//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// __ehvec_...() tests
// MT-UNsafe
#include "context_static.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      ARRAY_LEN  = 64
    , UNEXPECTED = 5
    };

    class test
    {
      int val;
    public:
      typedef int value_type;
      static int ctor_throw;
      static int cctor_throw;
      enum { BASES = 0 };
      test()
        : val ( context::ctor_count )
      {
        if ( ctor_throw && ctor_throw == context::ctor_count)
        {
          throw ctor_throw;
        }
        context::ctor();
      }
      test(test const& src)
        : val ( src.val )
      {
        if ( cctor_throw && cctor_throw == context::cctor_count)
        {
          throw cctor_throw;
        }
        context::cctor();
      }
      /*virtual*/ ~test()
      {
        context::dtor();
      }
      int value() const
      {
        return val;
      }
    };
    int test::ctor_throw  = 0;
    int test::cctor_throw = 0;

    class vbtest : public virtual test
    {
    public:
      enum { BASES = 1 };
      vbtest()
        : test()
      {
        context::ctor();
      }
      vbtest(vbtest const& src)
        : test ( src )
      {
        context::cctor();
      }
      ~vbtest()
      {
        context::dtor();
      }
    };

    class ftbtest : public test
    {
    public:
      enum { BASES = 1 };
      ftbtest()
      try
        : test()
      {
        context::ctor();
      }
      catch (...)
      {
      #if defined (_MSC_VER) && (_MSC_VER < 1310)
        throw;
      #else
      // implicit `throw;'
      #endif
      }
      ftbtest(ftbtest const& src)
      try
        : test ( src )
      {
        context::cctor();
      }
      catch (...)
      {
      #if defined (_MSC_VER) && (_MSC_VER < 1310)
        throw;
      #else
      // implicit `throw;'
      #endif
      }
      ~ftbtest()
      {
        context::dtor();
      }
    };

    template <typename T>
    struct holder
    {
      T stuff;
    };

    template <unsigned L, typename T>
    void check(T const (&array)[L])
    {
      typename T::value_type sum = 0;
      for ( unsigned idx = 0; idx < L; ++idx )
      {
        sum += array[idx].value();
      }
      if ( sum != (array[0].value() + array[L-1].value()) * (L>>1) )
      {
        throw int(UNEXPECTED);
      }
    }

    template <unsigned L, typename T>
    void check(T const* const ptr)
    {
      check(*reinterpret_cast<T const(*)[L]>(ptr));
    }
  }  // namespace

//===============================================================================
// `__ehvec_ctor' and `__ehvec_copy_ctor' and `__ehvec_dtor'
// NOTE icl doesn't emit `__ehvec_copy_ctor', uses own inline array cctor instead
//-------------------------------------------------------------------------------
// `__ehvec_ctor_vb' and `__ehvec_copy_ctor_vb' and `__ehvec_dtor'
// NOTE icl doesn't emit `__ehvec_copy_ctor_vb',
//      icl>=icl15 uses own inline array cctor, while icl<icl15 crashes
//-------------------------------------------------------------------------------
  namespace
  {
    template <typename T>
    bool test_ehvec()
    {
      typedef T test_type;
      typedef holder<T[ARRAY_LEN]> holder_type;
      context::init(ARRAY_LEN + 2*ARRAY_LEN + 3*ARRAY_LEN);
      test::ctor_throw = 0;
      test::cctor_throw = 0;
      try
      {
        {
          holder_type h1;
          check(h1.stuff);
          {
            holder_type h2;
            check(h2.stuff);
          }
          holder_type h3;
          check(h3.stuff);
          try
          {
            throw h3.stuff;
          }
          catch (test_type const* const exc)
          {
            check<ARRAY_LEN>(exc);
            context::state += ARRAY_LEN;
          }
          try
          {
            throw holder_type();
          }
          catch (holder_type const& exc)
          {
            check(exc.stuff);
            context::state += 2*ARRAY_LEN;
          }
        }
        {
          holder_type h1;
          check(h1.stuff);
          try
          {
            holder_type h2(h1);
            check(h2.stuff);
            throw h2;
          }
          catch (holder_type const exc)
          {
            check(exc.stuff);
            context::state += 3*ARRAY_LEN;
          }
        }
      }
      catch (int const exc)
      {
        context::state = exc;
      }
      return context::ok();
    }
  }  // namespace

  bool test_5101()
  {
    return test_ehvec<test>();
  }

  bool test_5102()
  {
    return test_ehvec<vbtest>();
  }

//=============
// array unwind
//-------------
  namespace
  {
    template <typename T>
    bool test_array_unwind()
    {
      typedef T test_type;
      typedef holder<T[ARRAY_LEN]> holder_type;
      context::init(0);
      try
      {
        test::ctor_throw  = 0;
        test::cctor_throw = 0;
        holder_type h1;
        check(h1.stuff);
        test::ctor_throw  = context::ctor_count + (ARRAY_LEN>>1) * (test_type::BASES+1);
        context::expected += test::ctor_throw;
        holder_type h2;
      }
      catch (int const exc)
      {
        context::state += exc;
      }
      try
      {
        test::ctor_throw  = 0;
        test::cctor_throw = 0;
        holder_type h1;
        holder_type h2(h1);
        check(h2.stuff);
        test::cctor_throw = context::cctor_count + (ARRAY_LEN>>1) * (test_type::BASES+1);
        context::expected += 2*test::cctor_throw;
        holder_type h3(h2);
      }
      catch (int const exc)
      {
        context::state += 2*exc;
      }
      return context::ok();
    }
  }  // namespace

  bool test_5103()
  {
    return test_array_unwind<test>();
  }

  bool test_5104()
  {
    return test_array_unwind<vbtest>();
  }

  bool test_5105()
  {
    return test_array_unwind<ftbtest>();
  }

}}}  // namespace cpprtl::eh::test

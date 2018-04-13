/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
////
////   __ehvec_...() test for types with virtual inheritance
////   MT-UNsafe
/////////////////////////////////////////////////////////////////////////////


#include "context_static.h"


namespace
{
  enum
  {
    EH_OK               = 0
  , UNEXPECTED_CATCH_1  = 521
  , UNEXPECTED_CATCH_2  = 522
  , UNEXPECTED_CATCH_3  = 523
  , UNEXPECTED_CATCH_4  = 524
  , UNEXPECTED_CATCH_5  = 525
  , ARRAY_IS_CORRUPTED  = 526
  , ARRAY_OUT_OF_SCOPE  = 527
  , ARR_SZ              = 32
  };


  class base52
  {
    int volatile i52;

  public:
    base52()
      : i52 ( ++context::ctor_count )
    {
      ++context::xtor_count;
    }

    base52(base52 const& src)
      : i52 ( src.i52 )
    {
      ++context::cctor_count;
      ++context::xtor_count;
    }

    base52& operator=(base52 const& src)
    {
      if (this != &src)
      {
        i52 = src.i52;
      }
      return *this;
    }

    virtual ~base52()
    {
      i52 = -1;
      ++context::dtor_count;
      --context::xtor_count;
    }

    int get() const
    {
      return i52;
    }
  };


  class cvtest52
    : public virtual base52
  {
    typedef base52 vbase_t;
  public:
    cvtest52()
      : vbase_t ()
    {
      ++context::vctor_count;
      ++context::xtor_count;
    }

    cvtest52(cvtest52 const& src)
      : vbase_t ( src )
    {
      ++context::vcctor_count;
      ++context::xtor_count;
    }

    ~cvtest52()
    {
      ++context::vdtor_count;
      --context::xtor_count;
    }
  };


  typedef cvtest52 array_of_cvtest52[ARR_SZ];


  struct cvtest52_array_holder
  {
    array_of_cvtest52 array;
  };


  template <typename T>
  static void check(T const arr[])
  throw(...)
  {
    int sum = 0;
    for ( unsigned u = 0; u < ARR_SZ; ++u )
    {
      sum += arr[u].get();
    }
    if ( sum == 0 )
    {
      throw int(ARRAY_OUT_OF_SCOPE);
    }
    if ( sum != (arr[0].get() + arr[ARR_SZ-1].get()) * (ARR_SZ >> 1) )
    {
      throw int(ARRAY_IS_CORRUPTED);
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test52()
  {
    context::init();
    int res = EH_OK;

    try
    {
      {
      // this scope invokes '__ehvec_ctor_vb' and '__ehvec_dtor'
        array_of_cvtest52 varray1;
        check(varray1);
        {
          array_of_cvtest52 varray2;
          check(varray2);
        }
        array_of_cvtest52 varray3;
        check(varray3);

        try
        {
          throw varray3;
        }
      #if defined (_MSC_VER) && (_MSC_VER < 1310)  // ddk2600
        catch (cvtest52 const* const exc)
      #else
        catch (cvtest52 const* const& exc)
      #endif
        {
          check(exc);
        }
        catch (...)
        {
          return context::balance(UNEXPECTED_CATCH_2);
        }

        try
        {
          throw cvtest52_array_holder();
        }
        catch (cvtest52_array_holder const& exc)
        {
          check(exc.array);
        }
        catch (...)
        {
          return context::balance(UNEXPECTED_CATCH_3);
        }
      }

    // icl<icl15 seems not to handle '__ehvec_copy_ctor_vb' duties properly, so let's just skip the following test scope
    // icl>=icl15 generates some custom array copying code
    #if !defined (__ICL) || (__ICL >= 1500)
      {
      // this scope invokes '__ehvec_ctor_vb' and '__ehvec_copy_ctor_vb' and '__ehvec_dtor'
        cvtest52_array_holder vholder1;
        check(vholder1.array);
        cvtest52_array_holder vholder2(vholder1);
        check(vholder2.array);

        try
        {
          throw vholder1;
        }
        catch (cvtest52_array_holder const/*&*/ exc)
        {
          check(exc.array);
        }
        catch (...)
        {
          return context::balance(UNEXPECTED_CATCH_4);
        }

        try
        {
          cvtest52_array_holder arr;
          throw arr;
        }
        catch (cvtest52_array_holder const/*&*/ exc)
        {
          check(exc.array);
        }
        catch (...)
        {
          return context::balance(UNEXPECTED_CATCH_5);
        }
      }
    #endif  // __ICL
    }
    catch (int const& i)  // array check falls here if an inconsistency occured
    {
      res = i;
    }
    catch (...)
    {
      res = UNEXPECTED_CATCH_1;
    }
    return context::balance(res);
  }

}  }  }


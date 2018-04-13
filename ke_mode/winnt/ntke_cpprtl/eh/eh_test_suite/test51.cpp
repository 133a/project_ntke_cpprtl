/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
////
////   __ehvec_...() test for types with no inheritance
////   MT-UNsafe
/////////////////////////////////////////////////////////////////////////////


#include "context_static.h"


namespace
{
  enum
  {
    EH_OK               = 0
  , UNEXPECTED_CATCH_1  = 511
  , UNEXPECTED_CATCH_2  = 512
  , UNEXPECTED_CATCH_3  = 513
  , UNEXPECTED_CATCH_4  = 514
  , UNEXPECTED_CATCH_5  = 515
  , ARRAY_IS_CORRUPTED  = 516
  , ARRAY_OUT_OF_SCOPE  = 517
  , ARR_SZ              = 32
  };


  class ctest51
  {
    int volatile i51;

  public:
    ctest51()
      : i51 ( ++context::ctor_count )
    {
      ++context::xtor_count;
    }

    ctest51(ctest51 const& src)
      : i51 ( src.i51 )
    {
      ++context::cctor_count;
      ++context::xtor_count;
    }

    ctest51& operator=(ctest51 const& src)
    {
      if ( this != &src)
      {
        i51 = src.i51;
      }
      return *this;
    }

    ~ctest51()
    {
      i51 = 0;
      ++context::dtor_count;
      --context::xtor_count;
    }

    int get() const
    {
      return i51;
    }
  };


  typedef ctest51 array_of_ctest51[ARR_SZ];


  struct ctest51_array_holder
  {
    array_of_ctest51 array;
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

  int test51()
  {
    context::init();
    int res = EH_OK;

    try
    {
      {
      // this scope invokes '__ehvec_ctor' and '__ehvec_dtor'
        array_of_ctest51 array1;
        check(array1);
        {
          array_of_ctest51 array2;
          check(array2);
        }
        array_of_ctest51 array3;
        check(array3);

        try
        {
          throw array3;
        }
      #if defined (_MSC_VER) && (_MSC_VER < 1310)  // ddk2600
        catch (ctest51 const* const exc)
      #else
        catch (ctest51 const* const& exc)
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
          throw ctest51_array_holder();
        }
        catch (ctest51_array_holder const& exc)
        {
          check(exc.array);
        }
        catch (...)
        {
          return context::balance(UNEXPECTED_CATCH_3);
        }
      }
    
    // icl<icl15 seems not to handle '__ehvec_copy_ctor' duties properly, so let's just skip the following test scope
    // icl>=icl15 generates some custom array copying code
    #if !defined (__ICL) || (__ICL >= 1500)
      {
      // this scope invokes '__ehvec_ctor' and '__ehvec_copy_ctor' and '__ehvec_dtor'
        ctest51_array_holder holder1;
        check(holder1.array);
        ctest51_array_holder holder2(holder1);
        check(holder2.array);

        try
        {
          throw holder1;
        }
        catch (ctest51_array_holder const/*&*/ exc)
        {
          check(exc.array);
        }
        catch (...)
        {
          return context::balance(UNEXPECTED_CATCH_4);
        }

        try
        {
          ctest51_array_holder arr;
          throw arr;
        }
        catch (ctest51_array_holder const/*&*/ exc)
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


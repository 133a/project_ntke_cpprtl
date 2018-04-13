/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
////
////   __ehvec_...() unwinding test for types with no inheritance
////   MT-UNsafe
/////////////////////////////////////////////////////////////////////////////


#include "context_static.h"


namespace
{
  enum
  {
    EH_OK              = 0
  , SPECIAL_EXCEPTION  = 530
  , UNEXPECTED_ERROR   = 531
  , ARR_SZ             = 32
  };


  void throwing_func(int const& idx)
  {
    if ( ( idx % ARR_SZ ) > ( ARR_SZ >> 1 ) )
    {
      throw int(SPECIAL_EXCEPTION);
    }
  }


  static bool throw_ctor  = false;
  static bool throw_cctor = false;


  class ctest53
  {
  public:
    ctest53()
    {
      if ( throw_ctor )
      {
        throwing_func(context::ctor_count);
      }
      ++context::ctor_count;
      ++context::xtor_count;
    }

    ctest53(ctest53 const& src)
    {
      if ( throw_cctor )
      {
        throwing_func(context::cctor_count);
      }
      ++context::cctor_count;
      ++context::xtor_count;
    }

    ~ctest53()
    {
      ++context::dtor_count;
      --context::xtor_count;
    }
  };


  typedef ctest53 array_of_ctest53[ARR_SZ];


  struct ctest53_array_holder
  {
    array_of_ctest53 array;
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test53()
  {
    context::init();
    throw_ctor  = false;
    throw_cctor = false;
    int res = UNEXPECTED_ERROR;

    try
    {
      try
      {
      // this scope invokes '__ehvec_ctor' and '__ehvec_dtor'
        throw_ctor  = false;
        throw_cctor = false;
        array_of_ctest53 array1;
        throw_ctor  = true;
        array_of_ctest53 array2;
      }
      catch (int const& i)
      {
        if ( i != SPECIAL_EXCEPTION )
        {
          return context::balance(SPECIAL_EXCEPTION);
        }
        res = EH_OK;
      }
      catch (...)
      {
        return context::balance(UNEXPECTED_ERROR);
      }

    // icl<icl15 seems not to handle '__ehvec_copy_ctor' duties properly, so let's just skip the following test scope
    // icl>=icl15 generates some custom array copying code
    #if !defined (__ICL) || (__ICL >= 1500)
      res = UNEXPECTED_ERROR;
      try
      {
      // this scope invokes '__ehvec_ctor' and '__ehvec_copy_ctor' and '__ehvec_dtor'
        throw_ctor  = false;
        throw_cctor = false;
        ctest53_array_holder holder1;
        ctest53_array_holder holder2(holder1);
        throw_cctor = true;
        ctest53_array_holder holder3(holder2);
      }
      catch (int const& i)
      {
        if ( i != SPECIAL_EXCEPTION )
        {
          return context::balance(SPECIAL_EXCEPTION);
        }
        res = EH_OK;
      }
      catch (...)
      {
        return context::balance(UNEXPECTED_ERROR);
      }
    #endif  // __ICL
    }
    catch (...)
    {
      return context::balance(UNEXPECTED_ERROR);
    }
    return context::balance(res);
  }

}  }  }


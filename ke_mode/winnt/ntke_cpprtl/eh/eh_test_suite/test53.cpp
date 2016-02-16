/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
////
////   __ehvec_...() unwinding test for types with no inheritance
////   MT-UNsafe
/////////////////////////////////////////////////////////////////////////////


namespace
{
  enum
  {
    EH_OK                     = 0,
    UNEXPECTED_ERROR          = -1,
    SPECIAL_EXCEPTION         = -2,
    ARR_SZ                    = 6,
  };


  void throwing_func(int const& idx)
  {
    if ( (idx % ARR_SZ) > ARR_SZ/2 )
    {
      throw int(SPECIAL_EXCEPTION);
    }
  }

  static int ctor_count53    = 0;
  static int cctor_count53   = 0;
  static int dtor_count53    = 0;
  static int xtor_count53    = 0;

  static bool throw_ctor     = false;
  static bool throw_cctor    = false;
}


namespace
{

  class ctest53
  {
  public:
    ctest53()
    {
      if ( throw_ctor )
      {
        throwing_func(ctor_count53);
      }
      ++ctor_count53;
      ++xtor_count53;
    }

    ctest53(ctest53 const& src)
    {
      if ( throw_cctor )
      {
        throwing_func(cctor_count53);
      }
      ++cctor_count53;
      ++xtor_count53;
    }

    ~ctest53()
    {
      ++dtor_count53;
      --xtor_count53;
    }

  };


  typedef ctest53 array_of_ctest53[ARR_SZ];


  struct ctest53_array_holder
  {
    array_of_ctest53 array;
  };

}


namespace cpprtl { namespace test { namespace eh
{

  int test53()
  {
    ctor_count53    = 0;
    cctor_count53   = 0;
    dtor_count53    = 0;
    xtor_count53    = 0;
    throw_ctor      = false;
    throw_cctor     = false;

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
          return i;
        }
      }
      catch ( ... )
      {
        return UNEXPECTED_ERROR;
      }

#ifndef __ICL  // icl suddenly doesn't make use of '__ehvec_copy_ctor' so let's just skip the following test scope
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
          return i;
        }
      }
      catch ( ... )
      {
        return UNEXPECTED_ERROR;
      }
#endif

      res = EH_OK;
    }
    catch ( ... )
    {
      return UNEXPECTED_ERROR;
    }
  
    return res | ( xtor_count53 + (ctor_count53 + cctor_count53 - dtor_count53) );
  }

}  }  }


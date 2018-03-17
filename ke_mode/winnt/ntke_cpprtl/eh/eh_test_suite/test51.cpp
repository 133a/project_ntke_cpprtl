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


namespace
{
  enum
  {
    UNEXPECTED_ERROR          = -1,
    ARRAY_IS_CORRUPTED        = -2,
    ARR_SZ                    = 4,
  };
}


namespace
{
  static int ctor_count51    = 0;
  static int cctor_count51   = 0;
  static int dtor_count51    = 0;
  static int xtor_count51    = 0;


  class ctest51
  {
    int i;

  public:
    ctest51()
      : i ( ++ctor_count51 )
    {
      ++xtor_count51;
    }

    ctest51(ctest51 const& src)
      : i ( src.i )
    {
      ++cctor_count51;
      ++xtor_count51;
    }

    ctest51& operator=(ctest51 const& src)
    {
      if ( this != &src)
      {
        i = src.i;
      }
      return *this;
    }

    ~ctest51()
    {
      ++dtor_count51;
      --xtor_count51;
    }

    int get() const
    {
      return i;
    }
  };


  typedef ctest51 array_of_ctest51[ARR_SZ];


  struct ctest51_array_holder
  {
    array_of_ctest51 array;
  };


  template <typename T>
  static void check(T arr[])
  throw(...)
  {
    unsigned sum = 0;
    for (unsigned u = 0; u < ARR_SZ; ++u)
    {
      sum += arr[u].get();
    }

    if ( sum != (arr[0].get() + arr[ARR_SZ-1].get()) * (ARR_SZ >> 1) )
    {
      throw int(ARRAY_IS_CORRUPTED);
    }

  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test51()
  {
    ctor_count51    = 0;
    cctor_count51   = 0;
    dtor_count51    = 0;
    xtor_count51    = 0;

    int res = 0;
  
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
      }
    
#ifndef __ICL  // icl suddenly doesn't make use of '__ehvec_copy_ctor' so let's just skip the following test scope
      {
      // this scope invokes '__ehvec_ctor' and '__ehvec_copy_ctor' and '__ehvec_dtor'
        ctest51_array_holder holder1;
        check(holder1.array);
        ctest51_array_holder holder2(holder1);
        check(holder2.array);
      }
#endif

    }
    catch (int const& i)  //  array check falls here if an inconsistency occured
    {
      res = i;
    }
    catch ( ... )
    {
      res = UNEXPECTED_ERROR;
    }
  
    return res | ( xtor_count51 + (ctor_count51 + cctor_count51 - dtor_count51) );
  }

}  }  }


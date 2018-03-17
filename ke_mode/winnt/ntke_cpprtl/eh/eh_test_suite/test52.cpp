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
  static int ctor_count52    = 0;
  static int cctor_count52   = 0;
  static int dtor_count52    = 0;
  static int vctor_count52   = 0;
  static int vcctor_count52  = 0;
  static int vdtor_count52   = 0;
  static int xtor_count52    = 0;


  class base52
  {
    int i;

  public:
    base52()
      : i ( ++ctor_count52 )
    {
      ++xtor_count52;
    }

    base52(base52 const& src)
      : i ( src.i )
    {
      ++cctor_count52;
      ++xtor_count52;
    }

    base52& operator=(base52 const& src)
    {
      if (this != &src)
      {
        i = src.i;
      }
      return *this;
    }

    virtual ~base52()
    {
      ++dtor_count52;
      --xtor_count52;
    }

    int get() const
    {
      return i;
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
      ++vctor_count52;
      ++xtor_count52;
    }

    cvtest52(cvtest52 const& src)
      : vbase_t ( src )
    {
      ++vcctor_count52;
      ++xtor_count52;
    }

    ~cvtest52()
    {
      ++vdtor_count52;
      --xtor_count52;
    }
  };


  typedef cvtest52 array_of_cvtest52[ARR_SZ];


  struct cvtest52_array_holder
  {
    array_of_cvtest52 array;
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

  int test52()
  {
    ctor_count52    = 0;
    cctor_count52   = 0;
    dtor_count52    = 0;
    vctor_count52   = 0;
    vcctor_count52  = 0;
    vdtor_count52   = 0;
    xtor_count52    = 0;

    int res = 0;

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
      }
    
#ifndef __ICL  // icl suddenly doesn't make use of '__ehvec_copy_ctor_vb' so let's just skip the following test scope
      {
      // this scope invokes '__ehvec_ctor_vb' and '__ehvec_copy_ctor_vb' and '__ehvec_dtor'
        cvtest52_array_holder vholder1;
        check(vholder1.array);
        cvtest52_array_holder vholder2(vholder1);
        check(vholder2.array);
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
  
    return res | ( xtor_count52 + (ctor_count52 + cctor_count52 + vctor_count52 + vcctor_count52 - dtor_count52 - vdtor_count52) );
  }

}  }  }


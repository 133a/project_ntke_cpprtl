/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
////
////   __ehvec_...() unwinding test for types with virtual inheritance
////   MT-UNsafe
/////////////////////////////////////////////////////////////////////////////


namespace
{
  enum
  {
    EH_OK                     = 0,
    UNEXPECTED_ERROR          = -1,
    SPECIAL_EXCEPTION         = -2,
    ARR_SZ                    = 8,
  };


  void throwing_func(int const& idx)
  {
    if ( ( idx % ARR_SZ ) > ( ARR_SZ >> 1 ) )
    {
      throw int(SPECIAL_EXCEPTION);
    }
  }

  static int ctor_count54    = 0;
  static int cctor_count54   = 0;
  static int dtor_count54    = 0;
  static int vctor_count54   = 0;
  static int vcctor_count54  = 0;
  static int vdtor_count54   = 0;
  static int xtor_count54    = 0;

  static bool throw_ctor_vb  = false;
  static bool throw_cctor_vb = false;


  class base54
  {
  public:
    base54()
    {
      ++ctor_count54;
      ++xtor_count54;
    }

    base54(base54 const& src)
    {
      ++cctor_count54;
      ++xtor_count54;
    }

    virtual ~base54()
    {
      ++dtor_count54;
      --xtor_count54;
    }
  };


  class cvtest54
    : public virtual base54
  {
    typedef base54 vbase_t;
  public:
    cvtest54()
      : vbase_t ()
    {
      if ( throw_ctor_vb )
      {
        throwing_func(vctor_count54);
      }
      ++vctor_count54;
      ++xtor_count54;
    }

    cvtest54(cvtest54 const& src)
      : vbase_t ( src )
    {
      if ( throw_cctor_vb )
      {
        throwing_func(vcctor_count54);
      }
      ++vcctor_count54;
      ++xtor_count54;
    }


    ~cvtest54()
    {
      ++vdtor_count54;
      --xtor_count54;
    }
  };


  typedef cvtest54 array_of_cvtest54[ARR_SZ];


  struct cvtest54_array_holder
  {
    array_of_cvtest54 array;
  };

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test54()
  {
    ctor_count54    = 0;
    cctor_count54   = 0;
    dtor_count54    = 0;
    vctor_count54   = 0;
    vcctor_count54  = 0;
    vdtor_count54   = 0;
    xtor_count54    = 0;
    throw_ctor_vb   = false;
    throw_cctor_vb  = false;

    int res = UNEXPECTED_ERROR;
    try
    {
      try
      {
      // this scope invokes '__ehvec_ctor_vb' and '__ehvec_dtor'
        throw_ctor_vb  = false;
        throw_cctor_vb = false;
        array_of_cvtest54 varray1;
        throw_ctor_vb  = true;
        array_of_cvtest54 varray2;
      }
      catch (int const& i)
      {
        if ( i != SPECIAL_EXCEPTION )
        {
          return SPECIAL_EXCEPTION;
        }
        res = EH_OK;
      }
      catch (...)
      {
        return UNEXPECTED_ERROR;
      }

    #ifndef __ICL  // icl suddenly doesn't make use of '__ehvec_copy_ctor_vb' so let's just skip the following test scope
      res = UNEXPECTED_ERROR;
      try
      {
      // this scope invokes '__ehvec_ctor_vb' and '__ehvec_copy_ctor_vb' and '__ehvec_dtor'
        throw_ctor_vb  = false;
        throw_cctor_vb = false;
        cvtest54_array_holder vholder1;
        cvtest54_array_holder vholder2(vholder1);
        throw_cctor_vb = true;
        cvtest54_array_holder vholder3(vholder2);
      }
      catch (int const& i)
      {
        if ( i != SPECIAL_EXCEPTION )
        {
          return SPECIAL_EXCEPTION;
        }
        res = EH_OK;
      }
      catch (...)
      {
        return UNEXPECTED_ERROR;
      }
    #endif
    }
    catch (...)
    {
      return UNEXPECTED_ERROR;
    }
    return res | ( xtor_count54 + (ctor_count54 + cctor_count54 + vctor_count54 + vcctor_count54 - dtor_count54 - vdtor_count54) );
  }

}  }  }


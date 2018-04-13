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


#include "context_static.h"


namespace
{
  enum
  {
    EH_OK              = 0
  , SPECIAL_EXCEPTION  = 540
  , UNEXPECTED_ERROR   = 541
  , ARR_SZ             = 32
  };


  void throwing_func(int const& idx)
  {
    if ( ( idx % ARR_SZ ) > ( ARR_SZ >> 1 ) )
    {
      throw int(SPECIAL_EXCEPTION);
    }
  }

  static bool throw_ctor_vb  = false;
  static bool throw_cctor_vb = false;


  class base54
  {
  public:
    base54()
    {
      ++context::ctor_count;
      ++context::xtor_count;
    }

    base54(base54 const& src)
    {
      ++context::cctor_count;
      ++context::xtor_count;
    }

    virtual ~base54()
    {
      ++context::dtor_count;
      --context::xtor_count;
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
        throwing_func(context::vctor_count);
      }
      ++context::vctor_count;
      ++context::xtor_count;
    }

    cvtest54(cvtest54 const& src)
      : vbase_t ( src )
    {
      if ( throw_cctor_vb )
      {
        throwing_func(context::vcctor_count);
      }
      ++context::vcctor_count;
      ++context::xtor_count;
    }

    ~cvtest54()
    {
      ++context::vdtor_count;
      --context::xtor_count;
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
    context::init();
    throw_ctor_vb  = false;
    throw_cctor_vb = false;
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
          return context::balance(SPECIAL_EXCEPTION);
        }
        res = EH_OK;
      }
      catch (...)
      {
        return context::balance(UNEXPECTED_ERROR);
      }

    // icl<icl15 seems not to handle '__ehvec_copy_ctor_vb' duties properly, so let's just skip the following test scope
    // icl>=icl15 generates some custom array copying code
    #if !defined (__ICL) || (__ICL >= 1500)
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


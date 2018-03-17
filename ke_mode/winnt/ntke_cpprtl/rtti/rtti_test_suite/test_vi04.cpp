/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  virtual inheritance tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>
#include "rtti_test_defs.h"


namespace
{
  int const A_VAL = 783786;


  class base000 { public : int a; virtual ~base000() {} base000() : a(A_VAL) {} };

  class base10 : virtual private base000 { int a; };
  class base11 :         public  base000 { int a; };

  class derived : public base10, public base11 { int a; };

/**************************************************

   base000   base000
      v-       |
      v-       |
    base10   base11
        \     /
         \   /
          \ /
        derived

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{

  int test_vi04()
  {
    int ret = 0;
    derived d;

    try
    {

  //////  up-casts
      //  wrong up-cast
      //  bcc64 error: ambiguous conversion from derived class '<anonymous>::derived' to base class '<anonymous>::base000'
      ++ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base000* const b000_dyn = dynamic_cast<base000*>(&d);
        if ( b000_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


      //  up-cast
      ++ret;
      {
        base11* const b11_sta = &d;
        base000* const b000_sta = b11_sta;
        base000* const b000_dyn = dynamic_cast<base000*>(b11_sta);
        if ( b000_dyn != b000_sta )
        {
          throw ret;
        }
      }


  //////  down-casts
      //
      ++ret;
      {
        base000* const b000_sta = (base11*)&d;
        derived* const d_sta = &d;
        derived* const d_dyn = dynamic_cast<derived*>(b000_sta);
        if ( d_dyn != d_sta )
        {
          throw ret;
        }
      }


  //////  left-right cross-casts
      //  wrong left-right cross-cast
      //  icl error #312: conversion to inaccessible base class "<unnamed>::base000" is not allowed
      //  bcc64 error: cannot cast '<anonymous>::base10' to its private base class '<anonymous>::base000'
      ++ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base10* const b10_sta = &d;
        base000* const b000_dyn = dynamic_cast<base000*>(b10_sta);
        if ( b000_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


  //////  right-left cross-casts
      //  right-left cross-cast
      ++ret;
      {
        base11* const b11_sta = &d;
        base10* const b10_sta = &d;
        base10* const b10_dyn = dynamic_cast<base10*>(b11_sta);
        if ( b10_dyn != b10_sta )
        {
          throw ret;
        }
      }


      //  right-left cross-cast
      ++ret;
      {
        base000* const b000_sta = (base11*)&d;
        base10* const b10_sta = &d;
        base10* const b10_dyn = dynamic_cast<base10*>(b000_sta);
        if ( b10_dyn != b10_sta )
        {
          throw ret;
        }
      }

      ret = 0;  // one would get here only if tests were passed successfully
    }
    catch (int& i)
    {
      ret = i;
    }
    catch (std::bad_cast&)
    {
      ret = -ret;
    }
    catch (std::bad_typeid&)
    {
      ret = -ret;
    }
    catch (...)
    {
      ret = -ret;
    }
    return ret;
  }


}  }  }


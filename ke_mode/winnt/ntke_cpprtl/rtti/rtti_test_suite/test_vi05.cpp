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
  int const A_VAL = 43678;


  class base000 { public: int a; virtual ~base000(){} base000() : a(A_VAL) {} };

  class base00 : public base000 { int a; };

  class base10
    : virtual
    #ifdef MSVC_2015U3_FIXED_RTTI
      protected
    #else
      private
    #endif
      base00
  { int a; };

  class base11
    : virtual
    #ifdef MSVC_2015U3_FIXED_RTTI
      protected
    #else
      private
    #endif
      base00
    , public base000
  { int a; };

  class derived : public base10, public base11 { public: int a; };

/**************************************************

        base000
           |
           |
         base00  base000
           --     /
          -  -   /
         -    - /
     base10   base11
         \    /
          \  /
           \/
         derived

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{

  int test_vi05()
  {
    int ret = 0;
    derived d;

    try
    {

  //////  down-casts
      //  down-cast
      //  bcc64 error: ambiguous conversion from derived class '<anonymous>::base11' to base class '<anonymous>::base000'
      ++ret;
#if !( defined (__ICL) && __ICL < 1200 )  &&  !defined (__CODEGEARC__)
      {
        base000* const b000_sta = (base000*)(base11*)&d;
        derived* const d_sta = &d;
        derived* const d_dyn = dynamic_cast<derived*>(b000_sta);
        if ( d_dyn != d_sta )
        {
          throw ret;
        }
      }
#endif


      //  wrong down-cast (but possible with the mscl rtti data and lib)
      ++ret;
      {
        base000* const b000_sta = (base00*)(base11*)&d;
        base10* const b10_dyn = dynamic_cast<base10*>(b000_sta);
        base10* const b10_sta = &d;
#if defined (_MSC_VER)
        if ( b10_dyn != b10_sta )
#else
        if ( b10_dyn != 0 )
#endif
        {
          throw ret;
        }
      }


  //////  left-right cross-casts
      //  wrong left-right cross-cast (ambiguous target)
      //  icl error #312: conversion to inaccessible base class "<unnamed>::base00" is not allowed
      //  bcc64 error: ambiguous conversion from derived class '<anonymous>::base11' to base class '<anonymous>::base000'
      ++ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base10* const b10_sta = &d;
        base000* const b000_sta = (base11*)&d;
        base000* const b000_dyn = dynamic_cast<base000*>(b10_sta);
        if ( b000_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


  //////  right-left cross-casts
      //  right-left cross-cast
      //  bcc64 error: ambiguous conversion from derived class '<anonymous>::base11' to base class '<anonymous>::base000'
      ++ret;
#if !( defined (__ICL) && __ICL < 1200 )  &&  !defined (__CODEGEARC__)
      {
        base000* const b000_sta = (base11*)&d;
        base10* const b10_sta = &d;
        base10* const b10_dyn = dynamic_cast<base10*>(b000_sta);
        if ( b10_dyn != b10_sta )
        {
          throw ret;
        }
      }
#endif

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


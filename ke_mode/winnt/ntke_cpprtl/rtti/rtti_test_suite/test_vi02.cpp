/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  virtual private/protected inheritance tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>
#include "rtti_test_defs.h"


namespace
{
  int const A_VAL = 67778;


  class base00 { public : int a; virtual ~base00() {} base00() : a(A_VAL) {} };

  class base10 : virtual private    base00 { int a; virtual void b10() {} };  // virtual function is put to force the icl to generate rtti info
  class base11 : virtual protected  base00 { int a; virtual void b11() {} };  // virtual function is put to force the icl to generate rtti info
  class base12 : virtual public     base00 { int a; virtual void b12() {} };  // virtual function is put to force the icl to generate rtti info

  class derived : public base10, public base11, public base12 { int a; };

/**************************************************

                 base00
                  -+\
                 - + \
                -  +  \
               -   +   \
          base10 base11 base12
               \   |   /
                \  |  /
                 \ | /
                  \|/
                derived
        

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{

  int test_vi02()
  {
    int ret = 0;
    derived d;

    try
    {

      // cast-to-void*
      ++ ret;
      {
        base10* const b10_sta = &d;
        base11* const b11_sta = &d;
        base12* const b12_sta = &d;
        derived* const d_sta  = &d;
        void* const p10_dyn = dynamic_cast<void*>(b10_sta);
        void* const p11_dyn = dynamic_cast<void*>(b11_sta);
        void* const p12_dyn = dynamic_cast<void*>(b12_sta);
        void* const pd_dyn  = dynamic_cast<void*>(d_sta);

        if
        (
           ! p10_dyn
          || p10_dyn != p11_dyn
          || p11_dyn != p12_dyn
          || p12_dyn != pd_dyn
        )
        {
          throw ret;
        }
      }

    
      // down-cast
      ++ ret;
      {
        base12* const b12_sta = &d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b12_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }

    
      // down-cast
      ++ ret;
      {
        base10* const b10_sta = &d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b10_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }

    
      // left-right cross-cast
      ++ ret;
      {
        base10* const b10_sta = &d;
        base11* const b11_sta = &d;
        base11* const b11_dyn = dynamic_cast<base11*>(b10_sta);

        if ( b11_dyn != b11_sta )
        {
          throw ret;
        }
      }

    
      // right-left cross-cast
      ++ ret;
      {
        base12* const b12_sta = &d;
        base10* const b10_sta = &d;
        base10* const b10_dyn = dynamic_cast<base10*>(b12_sta);

        if ( b10_dyn != b10_sta )
        {
          throw ret;
        }
      }


      // cast to virtual base
      ++ ret;
      {
        base10* const b10_sta = &d;
        base11* const b11_sta = &d;
        derived* const d_sta  = &d;

        base00* const b00_dyn1 = dynamic_cast<base00*>(d_sta);
// icl error #312: conversion to inaccessible base class "<unnamed>::base00" is not allowed
// bcc64 error: cannot cast '<anonymous>::base10' to its private base class '<anonymous>::base00'
#if !defined (__ICL) && !defined (__CODEGEARC__)
        base00* const b00_dyn2 = dynamic_cast<base00*>(b10_sta);
        base00* const b00_dyn3 = dynamic_cast<base00*>(b11_sta);
#endif

        if
        (
             b00_dyn1 == 0
#if !defined (__ICL) && !defined (__CODEGEARC__)
          || b00_dyn1 != b00_dyn2
          || b00_dyn2 != b00_dyn3
#endif
        )
        {
          throw ret;
        }

        try
        {
          if ( A_VAL != b00_dyn1->a)
          {
            throw ret;
          }
        }
        catch (int&)
        {
          throw;
        }
        catch (...)
        {
          throw ret;
        }
      }


      // cast to void from virtual base
      ++ ret;
      {
        derived* const d_sta  = &d;
        base00* const b00_dyn = dynamic_cast<base00*>(d_sta);
        void* const p = dynamic_cast<void*>(b00_dyn);
        if ( p != d_sta )
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


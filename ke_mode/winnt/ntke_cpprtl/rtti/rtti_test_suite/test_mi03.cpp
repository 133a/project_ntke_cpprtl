/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  multiple ambiguous inheritance tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>


namespace
{

  class base00 { int a; public : virtual ~base00() {} };
  class base100 : public base00 { int a; };

  class base20 : public  base100  { int a; };
  class base21 : public  base100  { int a; };
  class base22 : private base100 { int a; };

  class base300 : public base20, public base21 { int a; };
  class base32  : public base22 { int a; };

  class derived : public base300, public base32 { int a; };

/**************************************************

  base00  base00  base00
    |       |       |
  base100 base100 base100
    |       |       -
  base20  base21  base22
     \     |        |
      \    |      base32
       \   |      /  
        \  |     /   
       base300  /    
          |    /
         derived

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{


  int test_mi03()
  {
    int ret = 0;
    derived d;

    try
    {

  ////// cast-to-void*
      ++ret;
      {
        base20* const b20_sta     = &d;
        base21* const b21_sta     = &d;
        base22* const b22_sta     = &d;
        base100* const b100_sta0  = b20_sta;
        base100* const b100_sta1  = b21_sta;
        base100* const b100_sta2  = (base100*)b22_sta;
        base300* const b300_sta   = &d;
        base32* const b32_sta     = &d;
        derived* const d_sta      = &d;

        void* const p100_dyn0  = dynamic_cast<void*>(b100_sta0);
        void* const p100_dyn1  = dynamic_cast<void*>(b100_sta1);
        void* const p100_dyn2  = dynamic_cast<void*>(b100_sta2);
        void* const p20_dyn    = dynamic_cast<void*>(b20_sta);
        void* const p21_dyn    = dynamic_cast<void*>(b21_sta);
        void* const p22_dyn    = dynamic_cast<void*>(b22_sta);
        void* const p300_dyn   = dynamic_cast<void*>(b300_sta);
        void* const p32_dyn    = dynamic_cast<void*>(b32_sta);
        void* const pd_dyn     = dynamic_cast<void*>(d_sta);

        if
        (
           ! p100_dyn0
          || p100_dyn0 != p100_dyn1
          || p100_dyn1 != p100_dyn2
          || p100_dyn2 != p20_dyn
          || p20_dyn   != p21_dyn
          || p21_dyn   != p22_dyn
          || p22_dyn   != p300_dyn
          || p300_dyn  != p32_dyn
          || p32_dyn   != pd_dyn
        )
        {
          throw ret;
        }
      }


  ////// down-casts
      // down-cast
      ++ ret;
      {
        base20* const b20_sta = &d;
        base00* const b00_sta0 = b20_sta;
        derived* const d_sta = &d;
        derived* const d_dyn = dynamic_cast<derived*>(b00_sta0);

        if ( d_dyn != d_sta )
        {
          throw ret;
        }
      }

    
      // wrong down-cast
      // TODO icl fails
      ++ ret;
#ifndef  __ICL
      {
        base32* const b32_sta = &d;
        base00* const b00_sta2 = (base00*)b32_sta;
        derived* const d_dyn = dynamic_cast<derived*>(b00_sta2);

        if ( d_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


  ////// up-casts
      // compile-time up-cast
      ++ ret;
      {
        base21* const b21_sta = &d;
        base00* const b00_sta1 = b21_sta;
        base00* const b00_dyn = dynamic_cast<base00*>(b21_sta);

        if ( b00_dyn != b00_sta1 )
        {
          throw ret;
        }
      }

    
      // wrong up-cast
      // icl error: base class "<unnamed>::base100" is ambiguous
      // bcc64 error: ambiguous conversion from derived class '<anonymous>::base300' to base class '<anonymous>::base100'
      ++ ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base300* const b300_sta = &d;
        base100* const b100_dyn = dynamic_cast<base100*>(b300_sta);

        if ( b100_dyn != 0 )
        {
          throw ret;
        }
      }
#endif
    

  ////// left-right cross-casts
      // left-right cross-cast
      ++ ret;
      {
        base20* const b20_sta = &d;
        base00* const b00_sta0 = b20_sta;
        base22* const b22_sta = &d;
        base22* const b22_dyn = dynamic_cast<base22*>(b00_sta0);

        if ( b22_dyn != b22_sta )
        {
          throw ret;
        }
      }

    
  ////// right-left cross-casts
      // right-left cross-cast
      ++ ret;
      {
        base21* const b21_sta = &d;
        base00* const b00_sta1 = b21_sta;
        base20* const b20_sta = &d;
        base20* const b20_dyn = dynamic_cast<base20*>(b00_sta1);

        if ( b20_dyn != b20_sta )
        {
          throw ret;
        }
      }


      // wrong right-left cross-cast
      // icl error #312: conversion to inaccessible base class "<unnamed>::base100" is not allowed
      // bcc64 error: cannot cast '<anonymous>::base22' to its private base class '<anonymous>::base100'
      ++ ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base22* const b22_sta = &d;
        base100* const b100_dyn = dynamic_cast<base100*>(b22_sta);

        if ( b100_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


      // wrong right-left cross-cast
      // TODO icl fails
      ++ ret;
#ifndef  __ICL
      {
        base32* const b32_sta = &d;
        base00* const b00_sta2 = (base00*)b32_sta;
        base21* const b21_dyn = dynamic_cast<base21*>(b00_sta2);

        if ( b21_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


      ret = 0; // one would go here only if tests are successfully passed
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


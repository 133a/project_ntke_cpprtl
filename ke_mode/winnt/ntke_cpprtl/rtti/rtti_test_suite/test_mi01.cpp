/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  multiple public inheritance tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>
#include "rtti_test_defs.h"


namespace
{

  class base00 { int a; public : virtual ~base00() {} };
  class base01 { int a; public : virtual ~base01() {} };
  class base02 { int a; public : virtual ~base02() {} };

  class base10 : public base00 { int a; };
  class base11 : public base01 { int a; };
  class base12 : public base02 { int a; };

  class base20 : public base10 { int a; };
  class base21 : public base11 { int a; };
  class base22 : public base12 { int a; };

  class base30 : public base20 { int a; };
  class base32 : public base22 { int a; };

  class base42 : public base32 { int a; };

  class base52 : public base42 { int a; };

  class derived : public base30, public base21, public base52 { int a; };

/**************************************************

  base00  base01  base02
    |       |       |
  base10  base11  base12
    |       |       |
  base20  base21  base22
    |       |       |
  base30    |     base32
    |       |       |
    |       |     base42
    |       |       |
    |       |     base52
    |       |       |
    --------+--------
            |
         derived

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{


  int test_mi01()
  {
    int ret = RTTI_OK;
    derived d;

    try
    {

      // cast-to-void*
      ++ret;
      {
        base10* const b10_sta = &d;
        base11* const b11_sta = &d;
        base20* const b20_sta = &d;
        base21* const b21_sta = &d;
        base32* const b32_sta = &d;
        base52* const b52_sta = &d;
        derived* const d_sta  = &d;
        void* const p10_dyn = dynamic_cast<void*>(b10_sta);
        void* const p11_dyn = dynamic_cast<void*>(b11_sta);
        void* const p20_dyn = dynamic_cast<void*>(b20_sta);
        void* const p21_dyn = dynamic_cast<void*>(b21_sta);
        void* const p32_dyn = dynamic_cast<void*>(b32_sta);
        void* const p52_dyn = dynamic_cast<void*>(b52_sta);
        void* const pd_dyn  = dynamic_cast<void*>(d_sta);

        if
        (
           ! p10_dyn
          || p10_dyn != p11_dyn
          || p11_dyn != p20_dyn
          || p20_dyn != p21_dyn
          || p21_dyn != p32_dyn
          || p32_dyn != p52_dyn
          || p52_dyn != pd_dyn
        )
        {
          throw ret;
        }
      }


      // down-cast
      ++ret;
      {
        base00* const b00_sta = &d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b00_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }

    
      // left-right cross-cast
      ++ret;
      {
        base20* const b20_sta = &d;
        base02* const b02_sta = &d;
        base02* const b02_dyn = dynamic_cast<base02*>(b20_sta);

        if ( b02_dyn != b02_sta )
        {
          throw ret;
        }
      }

    
      // right-left cross-cast
      ++ret;
      {
        base22* const b22_sta = &d;
        base01* const b01_sta = &d;
        base01* const b01_dyn = dynamic_cast<base01*>(b22_sta);

        if ( b01_dyn != b01_sta )
        {
          throw ret;
        }
      }

      ret = RTTI_OK;  // one would get here only if tests passed successfully
    }
    catch (int& i)
    {
      ret = i;
    }
    catch (std::bad_cast&)
    {
      ret += ERROR_BAD_CAST;
    }
    catch (std::bad_typeid&)
    {
      ret += ERROR_BAD_TYPEID;
    }
    catch (...)
    {
      ret += ERROR_UNEXPECTED;
    }
    return ret;
  }


}  }  }


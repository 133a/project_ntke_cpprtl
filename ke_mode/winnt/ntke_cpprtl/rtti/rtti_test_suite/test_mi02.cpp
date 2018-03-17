/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  private/protected multiple inheritance tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>
#include "rtti_test_defs.h"


namespace
{
  class base000 { int a; public : virtual ~base000() {} };
  class base001 { int a; public : virtual ~base001() {} };
  class base002 { int a; public : virtual ~base002() {} };


  class base00 { int a; public : virtual ~base00() {} };
  class base01 : protected base000, public base001, public base002 { int a; public : virtual ~base01() {} };
  class base02 { int a; public : virtual ~base02() {} };

  class base10 : private    base00 { int a; };
  class base11 : public     base01 { int a; };
  class base12 : public     base02 { int a; };

  class base20 : public     base10 { int a; };
  class base21 : public     base11 { int a; };
  class base22 : private    base12 { int a; };

  class base30 : public     base20 { int a; };
  class base32 : protected  base22 { int a; };

  class base42 : public     base32 { int a; };

  class base52 : public     base42 { int a; };

  class derived : public base30, public base21, public base52 { int a; };

/**************************************************


  base000 base001 base002
      +     |     /
       +    |    /
        +   |   /
         +  |  /
          + | /
           +|/
  base00  base01  base02
    -       |       |
  base10  base11  base12
    |       |       -
  base20  base21  base22
    |       |       +
  base30    |     base32
    |       |       |
    |       |     base42
    |       |       |
    |       |     base52
    |       |       |
    --------|--------
            |
         derived

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{


  int test_mi02()
  {
    int ret = 0;
    derived d;

    try
    {

  ////// cast-to-void*
      ++ret;
      {
        base000* const b000_sta = (base000*)&d;
        base002* const b002_sta = (base002*)&d;
        base00* const b00_sta = (base00*)&d;
        base10* const b10_sta = &d;
        base11* const b11_sta = &d;
        base12* const b12_sta = (base12*)&d;
        base20* const b20_sta = &d;
        base21* const b21_sta = &d;
        base32* const b32_sta = &d;
        base52* const b52_sta = &d;
        derived* const d_sta  = &d;
        void* const p000_dyn = dynamic_cast<void*>(b000_sta);
        void* const p002_dyn = dynamic_cast<void*>(b002_sta);
        void* const p00_dyn = dynamic_cast<void*>(b00_sta);
        void* const p10_dyn = dynamic_cast<void*>(b10_sta);
        void* const p11_dyn = dynamic_cast<void*>(b11_sta);
        void* const p12_dyn = dynamic_cast<void*>(b12_sta);
        void* const p20_dyn = dynamic_cast<void*>(b20_sta);
        void* const p21_dyn = dynamic_cast<void*>(b21_sta);
        void* const p32_dyn = dynamic_cast<void*>(b32_sta);
        void* const p52_dyn = dynamic_cast<void*>(b52_sta);
        void* const pd_dyn  = dynamic_cast<void*>(d_sta);

        if
        (
           ! p000_dyn
          || p000_dyn != p002_dyn
          || p002_dyn != p00_dyn
          || p00_dyn != p10_dyn
          || p10_dyn != p11_dyn
          || p11_dyn != p12_dyn
          || p12_dyn != p20_dyn
          || p20_dyn != p21_dyn
          || p21_dyn != p32_dyn
          || p32_dyn != p52_dyn
          || p52_dyn != pd_dyn
        )
        {
          throw ret;
        }
      }


  ////// down-casts
      // down-cast
      ++ ret;
      {
        base002* const b002_sta = &d;
        base21* const b21_sta = &d;
        base21* const b21_dyn = dynamic_cast<base21*>(b002_sta);

        if ( b21_dyn != b21_sta )
        {
          throw ret;
        }

      }

    
      // wrong down-cast
      ++ ret;
      {
        base000* const b000_sta = (base000*)&d;
        base01* const b01_dyn = dynamic_cast<base01*>(b000_sta);
    #ifdef MSVC_OLD_STYLE_RTTI
        base01* const b01_sta = &d;
        if (b01_dyn != b01_sta)
    #else
        if ( b01_dyn != 0 )
    #endif
        {
          throw ret;
        }

      }

    
      // down-cast
      ++ ret;
      {
        base02* const b02_sta = (base02*)&d;
        base12* const b12_sta = (base12*)&d;
        base12* const b12_dyn = dynamic_cast<base12*>(b02_sta);

        if ( b12_dyn != b12_sta )
        {
          throw ret;
        }

      }

    
      // wrong down-cast
      ++ ret;
      {
        base00* const b00_sta = (base00*)&d;
        derived* const pd_dyn = dynamic_cast<derived*>(b00_sta);

        if ( pd_dyn != 0 )
        {
          throw ret;
        }

      }


  ////// left-right cross-casts
      // left-right cross-cast
      ++ ret;
      {
        base001* const b001_sta = &d;
        base002* const b002_sta = &d;
        base002* const b002_dyn = dynamic_cast<base002*>(b001_sta);

        if ( b002_dyn != b002_sta )
        {
          throw ret;
        }
      }

    
      // wrong left-right cross-cast
      ++ ret;
      {
        base000* const b000_sta = (base000*)&d;
        base002* const b002_dyn = dynamic_cast<base002*>(b000_sta);

        if ( b002_dyn != 0 )
        {
          throw ret;
        }
      }

    
      // left-right cross-cast
      ++ ret;
      {
        base10* const b10_sta = &d;
        base32* const b32_sta = &d;
        base32* const b32_dyn = dynamic_cast<base32*>(b10_sta);

        if ( b32_dyn != b32_sta )
        {
          throw ret;
        }
      }


      // wrong left-right cross-cast
      ++ ret;
      {
        base00* const b00_sta = (base00*)&d;
        base22* const b22_dyn = dynamic_cast<base22*>(b00_sta);

        if ( b22_dyn != 0 )
        {
          throw ret;
        }
      }


      // wrong left-right cross-cast
      // TODO icl fails
      ++ ret;
#ifndef __ICL
      {
        base30* const b30_sta = &d;
        base02* const b02_dyn = dynamic_cast<base02*>(b30_sta);

        if ( b02_dyn != 0 )
        {
          throw ret;
        }
      }
#endif

   
  ////// right-left cross-casts
      // right-left cross-cast
      ++ ret;
      {
        base002* const b002_sta = &d;
        base001* const b001_sta = &d;
        base001* const b001_dyn = dynamic_cast<base001*>(b002_sta);

        if ( b001_dyn != b001_sta )
        {
          throw ret;
        }
      }

    
      // wrong right-left cross-cast
      ++ ret;
      {
        base002* const b002_sta = (base002*)&d;
        base000* const b000_dyn = dynamic_cast<base000*>(b002_sta);

        if ( b000_dyn != 0 )
        {
          throw ret;
        }
      }

    
      // right-left cross-cast
      ++ ret;
      {
        base42* const b42_sta = &d;
        base21* const b21_sta = &d;
        base21* const b21_dyn = dynamic_cast<base21*>(b42_sta);

        if ( b21_dyn != b21_sta )
        {
          throw ret;
        }
      }


      // wrong right-left cross-cast
      // TODO icl fails
      ++ ret;
#ifndef  __ICL
      {
        base02* const b02_sta = (base02*)&d;
        base20* const b20_dyn = dynamic_cast<base20*>(b02_sta);

        if ( b20_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


      // wrong right-left cross-cast
      ++ ret;
      {
        base32* const b32_sta = &d;
        base00* const b00_dyn = dynamic_cast<base00*>(b32_sta);

        if ( b00_dyn != 0 )
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


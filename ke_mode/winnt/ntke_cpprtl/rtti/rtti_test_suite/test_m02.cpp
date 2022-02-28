//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// multiple private/protected inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base000 { int a; int b; virtual ~base000() {} };
  struct base001 { int a; int b; int c; virtual ~base001() {} };
  struct base002 { int a; virtual ~base002() {} };

  struct base00
  { int a; virtual ~base00() {} };
  struct base01 : protected base000, public base001, public base002
  { int a; virtual ~base01() {} };
  struct base02
  { int a; virtual ~base02() {} };

  class base10 : private   base00 { int a; };
  class base11 : public    base01 { int a; int b; };
  class base12 : public    base02 { int a; };

  class base20 : public    base10 { int a; };
  class base21 : public    base11 { int a; };
  class base22 : private   base12 { int a; int b; };

  class base30 : public    base20 { int a; };
  class base32 : protected base22 { int a; int b; };

  class base42 : public    base32 { int a; int b; };

  class base52 : public    base42 { int a; };

  class derived : public base30, public base21, public base52 { int a; };

#if 0
  base000 base001 base002
      +     |     /
       +    |    /
        +   |   /
         +  |  /
          + | /
           +|/
  base00  base01 base02
    -       |      |
  base10  base11 base12
    |       |      -
  base20  base21 base22
    |       |      +
  base30    |    base32
    \       |      |
     \      |    base42
      \     |      |
       \    |    base52
        \   |     / 
         \  |    /  
          \ |   /
           \|  /
          derived
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_m0201()
  {
    derived d;
    base000* const pb000_sta = (base000*)&d;
    base002* const pb002_sta = (base002*)&d;
    base00* const  pb00_sta  = (base00*)&d;
    base10* const  pb10_sta  = &d;
    base11* const  pb11_sta  = &d;
    base12* const  pb12_sta  = (base12*)&d;
    base20* const  pb20_sta  = &d;
    base21* const  pb21_sta  = &d;
    base32* const  pb32_sta  = &d;
    base52* const  pb52_sta  = &d;
    derived* const pd_sta    = &d;
    void* const pdyn000 = dynamic_cast<void*>(pb000_sta);
    void* const pdyn002 = dynamic_cast<void*>(pb002_sta);
    void* const pdyn00  = dynamic_cast<void*>(pb00_sta);
    void* const pdyn10  = dynamic_cast<void*>(pb10_sta);
    void* const pdyn11  = dynamic_cast<void*>(pb11_sta);
    void* const pdyn12  = dynamic_cast<void*>(pb12_sta);
    void* const pdyn20  = dynamic_cast<void*>(pb20_sta);
    void* const pdyn21  = dynamic_cast<void*>(pb21_sta);
    void* const pdyn32  = dynamic_cast<void*>(pb32_sta);
    void* const pdyn52  = dynamic_cast<void*>(pb52_sta);
    void* const pdyn    = dynamic_cast<void*>(pd_sta);
    return
      eq (pdyn000, &d) &&
      eq (pdyn002, &d) &&
      eq (pdyn00, &d) &&
      eq (pdyn10, &d) &&
      eq (pdyn11, &d) &&
      eq (pdyn12, &d) &&
      eq (pdyn20, &d) &&
      eq (pdyn21, &d) &&
      eq (pdyn32, &d) &&
      eq (pdyn52, &d) &&
      eq (pdyn, &d);
  }

//==========
// down-cast
//----------
  bool test_m0202()
  {
    derived d;

    base10* const  pb10_sta  = &d;
    base30* const  pb30_dyn  = dynamic_cast<base30*>(pb10_sta);

    base002* const pb002_sta = &d;
    derived* const pd_dyn    = dynamic_cast<derived*>(pb002_sta);

    base02* const  pb02_sta  = (base02*)&d;
    base12* const  pb12_dyn  = dynamic_cast<base12*>(pb02_sta);

    base32* const  pb32_sta  = &d;
    base52* const  pb52_dyn  = dynamic_cast<base52*>(pb32_sta);

    return
      eq (pb30_dyn, static_cast<base30*>(&d)) &&
      eq (pd_dyn, &d) &&
      eq (pb12_dyn, (base12*)&d) &&
      eq (pb52_dyn, static_cast<base52*>(&d));
  }

//================
// wrong down-cast
//----------------
  bool test_m0203()
  {
    derived d;

    base00* const  pb00_sta  = (base00*)&d;
    derived* const pd_dyn    = dynamic_cast<derived*>(pb00_sta);

    base000* const pb000_sta = (base000*)&d;
    base11* const  pb11_dyn  = dynamic_cast<base11*>(pb000_sta);

    return
      eq (pd_dyn, 0) &&
    #if (MSVC_RTTI > MSVC_RTTI_V1)
      eq (pb11_dyn, 0)
    #else
      eq (pb11_dyn, static_cast<base11*>(&d))  // TODO falsely completed down-cast from inaccessible
    #endif
    ;
  }

//======================
// left-right cross-cast
//----------------------
  bool test_m0204()
  {
    derived d;

    base001* const pb001_sta = &d;
    base002* const pb002_dyn = dynamic_cast<base002*>(pb001_sta);

    base002* const pb002_sta = &d;
    base32* const  pb32_dyn  = dynamic_cast<base32*>(pb002_sta);

    base10* const  pb10_sta  = &d;
    base001* const pb001_dyn = dynamic_cast<base001*>(pb10_sta);

    base20* const  pb20_sta = &d;
    base42* const  pb42_dyn = dynamic_cast<base42*>(pb20_sta);

    return
      eq (pb002_dyn, static_cast<base002*>(&d)) &&
      eq (pb32_dyn, static_cast<base32*>(&d)) &&
      eq (pb001_dyn, static_cast<base001*>(&d)) &&
      eq (pb42_dyn, static_cast<base42*>(&d));
  }

//============================
// wrong left-right cross-cast
//----------------------------
  bool test_m0205()
  {
    derived d;

    base000* const pb000_sta = (base000*)&d;
    base002* const pb002_dyn = dynamic_cast<base002*>(pb000_sta);

    base00* const  pb00_sta  = (base00*)&d;
    base32* const  pb32_dyn  = dynamic_cast<base32*>(pb00_sta);

    base30* const  pb30_sta  = &d;
    base000* const pb000_dyn = dynamic_cast<base000*>(pb30_sta);

    base01* const  pb01_sta  = &d;
    base02* const  pb02_dyn  = dynamic_cast<base02*>(pb01_sta);

    return
      eq (pb002_dyn, 0) &&
      eq (pb32_dyn, 0) &&
      eq (pb000_dyn, 0) &&
      eq (pb02_dyn, 0);
  }

//======================
// right-left cross-cast
//----------------------
  bool test_m0206()
  {
    derived d;

    base002* const pb002_sta = &d;
    base001* const pb001_dyn = dynamic_cast<base001*>(pb002_sta);

    base001* const pb001_sta = &d;
    base10* const  pb10_dyn  = dynamic_cast<base10*>(pb001_sta);

    base32* const  pb32_sta  = &d;
    base21* const  pb21_dyn  = dynamic_cast<base21*>(pb32_sta);

    base52* const  pb52_sta  = &d;
    base20* const  pb20_dyn  = dynamic_cast<base20*>(pb52_sta);

    return
      eq (pb001_dyn, static_cast<base001*>(&d)) &&
      eq (pb10_dyn, static_cast<base10*>(&d)) &&
      eq (pb21_dyn, static_cast<base21*>(&d)) &&
      eq (pb20_dyn, static_cast<base20*>(&d));
  }

//============================
// wrong right-left cross-cast
//----------------------------
  bool test_m0207()
  {
    derived d;

    base002* const pb002_sta = (base002*)&d;
    base000* const pb000_dyn = dynamic_cast<base000*>(pb002_sta);

    base02* const  pb02_sta  = (base02*)&d;
    base20* const  pb20_dyn  = dynamic_cast<base20*>(pb02_sta);

    base32* const  pb32_sta  = &d;
    base00* const  pb00_dyn  = dynamic_cast<base00*>(pb32_sta);

    base000* const pb000_sta = (base000*)&d;
    base10* const  pb10_dyn  = dynamic_cast<base10*>(pb000_sta);

    return
      eq (pb000_dyn, 0) &&
      eq (pb20_dyn, 0) &&
      eq (pb00_dyn, 0) &&
      eq (pb10_dyn, 0);
  }

}}}  // namespace cpprtl::rtti::test

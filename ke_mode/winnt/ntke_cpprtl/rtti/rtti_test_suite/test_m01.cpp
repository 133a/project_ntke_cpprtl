//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// multiple public inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00 { int a; virtual ~base00() {} };
  struct base01 { int a; int b; virtual ~base01() {} };
  struct base02 { int a; int b; int c; virtual ~base02() {} };

  class base10 : public base00 { int a; };
  class base11 : public base01 { int a; int b; int c; };
  class base12 : public base02 { int a; };

  class base20 : public base10 { int a; };
  class base21 : public base11 { int a; };
  class base22 : public base12 { int a; int b; int c; };

  class base30 : public base20 { int a; int b; int c; };
  class base32 : public base22 { int a; };

  class base42 : public base32 { int a; int b; };

  class base52 : public base42 { int a; };

  class derived : public base30, public base21, public base52 { int a; };

#if 0
  base00 base01 base02
    |      |      |
  base10 base11 base12
    |      |      |
  base20 base21 base22
    |      |      |
  base30   |    base32
    |      |      |
    |      |    base42
    |      |      |
    \      |    base52
     \     |     /
      \    |    /
       \   |   /
        \  |  /
         \ | /
          \|/
        derived
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_m0101()
  {
    derived d;
    base10* const  pb10_sta = &d;
    base11* const  pb11_sta = &d;
    base20* const  pb20_sta = &d;
    base21* const  pb21_sta = &d;
    base32* const  pb32_sta = &d;
    base52* const  pb52_sta = &d;
    derived* const pd_sta   = &d;
    void* const    pdyn     = dynamic_cast<void*>(pd_sta);
    void* const    pdyn10   = dynamic_cast<void*>(pb10_sta);
    void* const    pdyn11   = dynamic_cast<void*>(pb11_sta);
    void* const    pdyn20   = dynamic_cast<void*>(pb20_sta);
    void* const    pdyn21   = dynamic_cast<void*>(pb21_sta);
    void* const    pdyn32   = dynamic_cast<void*>(pb32_sta);
    void* const    pdyn52   = dynamic_cast<void*>(pb52_sta);
    return
      eq (pd_sta, pdyn) &&
      eq (pd_sta, pdyn10) &&
      eq (pd_sta, pdyn11) &&
      eq (pd_sta, pdyn20) &&
      eq (pd_sta, pdyn21) &&
      eq (pd_sta, pdyn32) &&
      eq (pd_sta, pdyn52);
  }

//==========
// down-cast
//----------
  bool test_m0102()
  {
    derived d;

    base00* const  pb00_sta = &d;
    base20* const  pb20_dyn = dynamic_cast<base20*>(pb00_sta);

    base11* const  pb11_sta = &d;
    derived* const pd_dyn   = dynamic_cast<derived*>(pb11_sta);

    base22* const  pb22_sta = &d;
    base52* const  pb52_dyn = dynamic_cast<base52*>(pb22_sta);

    return
      eq (pb20_dyn, static_cast<base20*>(&d)) &&
      eq (pd_dyn, &d) &&
      eq (pb52_dyn, static_cast<base52*>(&d));
  }

//======================
// left-right cross-cast
//----------------------
  bool test_m0103()
  {
    derived d;

    base20* const pb20_sta = &d;
    base02* const pb02_dyn = dynamic_cast<base02*>(pb20_sta);

    base11* const pb11_sta = &d;
    base22* const pb22_dyn = dynamic_cast<base22*>(pb11_sta);

    return
      eq (pb02_dyn, static_cast<base02*>(&d)) &&
      eq (pb22_dyn, static_cast<base22*>(&d));
  }

//======================
// right-left cross-cast
//----------------------
  bool test_m0104()
  {
    derived d;
    base02* const pb02_sta = &d;
    base01* const pb01_dyn = dynamic_cast<base01*>(pb02_sta);
    base00* const pb00_dyn = dynamic_cast<base00*>(pb01_dyn);
    return
      eq (pb01_dyn, static_cast<base01*>(&d)) &&
      eq (pb00_dyn, static_cast<base00*>(&d));
  }

}}}  // namespace cpprtl::rtti::test

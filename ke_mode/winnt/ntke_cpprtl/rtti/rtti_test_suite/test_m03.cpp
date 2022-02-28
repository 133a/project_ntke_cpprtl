//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// multiple ambiguous inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{

  struct base00 { int a; virtual ~base00() {} };
  struct base01 { int a; int b; virtual ~base01() {} };

  class base10 : public base00 { int a; };
  class base11 : public base01 { int a; };

  class base20 : public  base10 { int a; };
  class base21 : public  base10 { int a; int b; };
  class base22 : private base11 { int a; int b; int c;};

  class base30 : public base20, public base21 { int a; };
  class base32 : public base22 { int a; int b; };

  class derived : public base30, public base32 { int a; };

#if 0
  base00 base00 base01
     |     |      |
  base10 base10 base11
     |     |      -
  base20 base21 base22
      \   /       |
      base30    base32
        |       /
        |      /
        |     /
        |    /
        |   /
       derived
#endif  // 0
}  // namespace
               
namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_m0301()
  {
    derived d;

    base20* const pb20_sta  = &d;
    void* const   pdyn20    = dynamic_cast<void*>(pb20_sta);

    base21* const pb21_sta  = &d;
    void* const   pdyn21    = dynamic_cast<void*>(pb21_sta);

    base22* const pb22_sta  = &d;
    void* const   pdyn22    = dynamic_cast<void*>(pb22_sta);

    base10* const pb10_sta0 = pb20_sta;
    void* const   pdyn0     = dynamic_cast<void*>(pb10_sta0);

    base10* const pb10_sta1 = pb21_sta;
    void* const   pdyn1     = dynamic_cast<void*>(pb10_sta1);

    base11* const pb11_sta  = (base11*)pb22_sta;
    void* const   pdyn2     = dynamic_cast<void*>(pb11_sta);

    base00* const pb00_sta0 = pb10_sta0;
    void* const   pdyn00    = dynamic_cast<void*>(pb00_sta0);

    base00* const pb00_sta1 = pb10_sta1;
    void* const   pdyn01    = dynamic_cast<void*>(pb00_sta1);

    base01* const pb01_sta  = pb11_sta;
    void* const   pdyn02    = dynamic_cast<void*>(pb01_sta);

    return
      eq (pdyn20, &d) &&
      eq (pdyn21, &d) &&
      eq (pdyn22, &d) &&
      eq (pdyn0, &d) &&
      eq (pdyn1, &d) &&
      eq (pdyn2, &d) &&
      eq (pdyn00, &d) &&
      eq (pdyn01, &d) &&
      eq (pdyn02, &d);
  }

//==========
// down-cast
//----------
  bool test_m0302()
  {
    derived d;

    base00* const  pb00_sta = static_cast<base21*>(&d);
    derived* const pd_dyn00 = dynamic_cast<derived*>(pb00_sta);
    base10* const  pb10_dyn = dynamic_cast<base10*>(pb00_sta);

    base10* const  pb10_sta = static_cast<base21*>(&d);
    base30* const  pb30_dyn = dynamic_cast<base30*>(pb10_sta);

    base22* const  pb22_sta = &d;
    derived* const pd_dyn22 = dynamic_cast<derived*>(pb22_sta);

    return
      eq (pd_dyn00, &d) &&
      eq (pb10_dyn, static_cast<base10*>(static_cast<base21*>(&d))) &&
      eq (pb30_dyn, static_cast<base30*>(&d)) &&
      eq (pd_dyn22, &d);
  }

//================
// wrong down-cast
//----------------
  bool test_m0303()
  {
    derived d;
    base01* const  pb01_sta = (base01*)static_cast<base32*>(&d);
    base22* const  pb22_dyn = dynamic_cast<base22*>(pb01_sta);
    derived* const pd_dyn   = dynamic_cast<derived*>(pb01_sta);
    return
    #if (MSVC_RTTI > MSVC_RTTI_V1)
      eq (pb22_dyn, 0) &&
    #else
      eq (pb22_dyn, static_cast<base22*>(&d)) &&  // TODO falsely completed down-cast from inaccessible
    #endif
      eq (pd_dyn, 0);
  }

//==============
// wrong up-cast
//--------------
  bool test_m0304()
  {
    derived d;

    base30* const pb30_sta = &d;
    base10* const pb10_dyn = dynamic_cast<base10*>(pb30_sta);  // msvc warning C4540

    base32* const pb32_sta = &d;
    base01* const pb01_dyn = dynamic_cast<base01*>(pb32_sta);  // msvc warning C4540

    return
      eq (pb10_dyn, 0) &&
      eq (pb01_dyn, 0);
  }

//======================
// left-right cross-cast
//----------------------
  bool test_m0305()
  {
    derived d;
    base00* const pb00_sta = static_cast<base20*>(&d);
    base21* const pb21_dyn = dynamic_cast<base21*>(pb00_sta);
    base22* const pb22_dyn = dynamic_cast<base22*>(pb00_sta);
    return
      eq (pb21_dyn, static_cast<base21*>(&d)) &&
      eq (pb22_dyn, static_cast<base22*>(&d));
  }

//============================
// wrong left-right cross-cast
//----------------------------
  bool test_m0306()
  {
    derived d;
    base00* const pb00_sta = static_cast<base20*>(&d);
    base11* const pb11_dyn = dynamic_cast<base11*>(pb00_sta);
    return
      eq (pb11_dyn, 0);
  }

//======================
// right-left cross-cast
//----------------------
  bool test_m0307()
  {
    derived d;

    base00* const pb00_sta = static_cast<base21*>(&d);
    base20* const pb20_dyn = dynamic_cast<base20*>(pb00_sta);

    base22* const pb22_sta = static_cast<base22*>(&d);
    base21* const pb21_dyn = dynamic_cast<base21*>(pb22_sta);

    return
      eq (pb20_dyn, static_cast<base20*>(&d)) &&
      eq (pb21_dyn, static_cast<base21*>(&d));
  }

//============================
// wrong right-left cross-cast
//----------------------------
  bool test_m0308()
  {
    derived d;

    base22* const pb22_sta = &d;
    base10* const pb10_dyn = dynamic_cast<base10*>(pb22_sta);

    base01* const pb01_sta = (base01*)static_cast<base32*>(&d);
    base21* const pb21_dyn = dynamic_cast<base21*>(pb01_sta);

    return
      eq (pb10_dyn, 0) &&
      eq (pb21_dyn, 0);
  }

}}}  // namespace cpprtl::rtti::test

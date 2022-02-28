//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// virtual inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00 { int a; virtual ~base00(){} };

  struct base10 : public base00 { int a; };
  struct base11 : public base00 { int a; int b; };

  class base20
    #if (MSVC_RTTI > MSVC_RTTI_V2)
    : virtual protected base10
    #else
    : virtual private   base10
    #endif
  { int a; };

  class base21
    #if (MSVC_RTTI > MSVC_RTTI_V2)
    : virtual protected base10
    #else
    : virtual private   base10
    #endif
    , public base11
  { int a; int b; };

  struct base31 { int a; virtual ~base31(){} };

  class derived : public base20, public base21, public base31 { public: int a; };

#if 0
      base00  base00
        |       |
        |       |
      base10  base11
       +-+-     /
      +-  +-   /
     +-    +- /
  base20   base21
     \      /
      \    /  base31 
       \  /   /
        \/   /
        derived
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{

//====================
// dynamic_cast<void*>
//--------------------
  bool test_v0501()
  {
    derived d;
    base31* const pb31_sta  = &d;
    base20* const pb20_sta  = &d;
    base21* const pb21_sta  = &d;
    base10* const pb10_sta  = (base10*)pb20_sta;
    base00* const pb00_sta0 = static_cast<base00*>(pb10_sta);
    base00* const pb00_sta1 = static_cast<base11*>(pb21_sta);
    void* const   pdyn31    = dynamic_cast<void*>(pb31_sta);
    void* const   pdyn20    = dynamic_cast<void*>(pb20_sta);
    void* const   pdyn21    = dynamic_cast<void*>(pb21_sta);
    void* const   pdyn10    = dynamic_cast<void*>(pb10_sta);
    void* const   pdyn000   = dynamic_cast<void*>(pb00_sta0);
    void* const   pdyn001   = dynamic_cast<void*>(pb00_sta1);
    void* const   pdyn      = dynamic_cast<void*>(&d);
    return
      eq (pdyn31, &d) &&
      eq (pdyn20, &d) &&
      eq (pdyn21, &d) &&
      eq (pdyn10, &d) &&
      eq (pdyn000, &d) &&
      eq (pdyn001, &d) &&
      eq (pdyn, &d);
  }

//==========
// down-cast
//----------
  bool test_v0502()
  {
    derived d;

    base00* const  pb00_sta1 = static_cast<base00*>(static_cast<base11*>(&d));
    derived* const pd_dyn1   = dynamic_cast<derived*>(pb00_sta1);

    // down-cast from inaccessible
    base10* const  pb10_sta  = (base10*)&d;
    base21* const  pb21_dyn  = dynamic_cast<base21*>(pb10_sta);

    base00* const  pb00_sta0 = static_cast<base00*>(pb10_sta);
    base10* const  pb10_dyn  = dynamic_cast<base10*>(pb00_sta0);
    base20* const  pb20_dyn  = dynamic_cast<base20*>(pb00_sta0);
    derived* const pd_dyn0   = dynamic_cast<derived*>(pb00_sta0);

    return
      eq (pd_dyn1, &d) &&
    #if (MSVC_RTTI > MSVC_RTTI_V1)
      eq (pb21_dyn, 0) &&
    #else
      eq (pb21_dyn, static_cast<base21*>(&d)) &&  // TODO falsely completed down-cast from inaccessible
    #endif
      eq (pb10_dyn, (base10*)&d) &&
      eq (pb20_dyn, static_cast<base20*>(&d)) &&  // TODO falsely completed down-cast from inaccessible
    #if (MSVC_RTTI > MSVC_RTTI_V0)
      eq (pd_dyn0, &d);  // TODO falsely completed down-cast from inaccessible
    #else
      eq (pd_dyn0, 0);
    #endif
  }

//======================
// left-right cross-cast
//----------------------
  bool test_v0503()
  {
    derived d;
    base20* const pb20_sta = &d;
    base11* const pb11_dyn = dynamic_cast<base11*>(pb20_sta);
    base31* const pb31_dyn = dynamic_cast<base31*>(pb20_sta);
    return
      eq (pb11_dyn, static_cast<base11*>(&d));
      eq (pb31_dyn, static_cast<base31*>(&d));
  }

//============================
// wrong left-right cross-cast
//----------------------------
  bool test_v0504()
  {
    derived d;

    base10* const pb10_sta = (base10*)static_cast<base20*>(&d);
    base11* const pb11_dyn = dynamic_cast<base11*>(pb10_sta);

    base20* const pb20_sta = &d;
    base00* const pb00_dyn = dynamic_cast<base00*>(pb20_sta);  // msvc warning C4540

    return
      eq (pb11_dyn, 0) &&
      eq (pb00_dyn, 0);
  }

//======================
// right-left cross-cast
//----------------------
  bool test_v0505()
  {
    derived d;

    base00* const pb00_sta = static_cast<base11*>(&d);
    base20* const pb20_dyn = dynamic_cast<base20*>(pb00_sta);

    base31* const pb31_sta = &d;
    base11* const pb11_dyn = dynamic_cast<base11*>(pb31_sta);

    return
      eq (pb20_dyn, static_cast<base20*>(&d)) &&
      eq (pb11_dyn, static_cast<base11*>(&d));
  }

//============================
// wrong right-left cross-cast
//----------------------------
  bool test_v0506()
  {
    derived d;

    base00* const pb00_sta = static_cast<base11*>(&d);
    base10* const pb10_dyn = dynamic_cast<base10*>(pb00_sta);

    base31* const pb31_sta = &d;
    base00* const pb00_dyn = dynamic_cast<base00*>(pb31_sta);

    return
      eq (pb10_dyn, 0) &&
      eq (pb00_dyn, 0);
  }

}}}  // namespace cpprtl::rtti::test

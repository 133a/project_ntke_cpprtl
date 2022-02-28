//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// virtual public inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00 { int a; virtual ~base00() {} };

  struct base10 : virtual public base00 { int a; virtual ~base10() {} };
  struct base11 : virtual public base00 { int a; int b; virtual ~base11() {} };

  class base20 : public base10 { int a; };
  class base21 : public base11 { int a; int b; };

  class derived : public base20, public base21 { int a; };

#if 0
      base00
        /\
       /  \
      /    \
  base10   base11
     |      |
  base20   base21
      \    /
       \  /
        \/
     derived
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_v0101()
  {
    derived d;
    base00* const pb00_sta = &d;
    base10* const pb10_sta = &d;
    base11* const pb11_sta = &d;
    base20* const pb20_sta = &d;
    base21* const pb21_sta = &d;
    void* const   pdyn00   = dynamic_cast<void*>(pb00_sta);
    void* const   pdyn10   = dynamic_cast<void*>(pb10_sta);
    void* const   pdyn11   = dynamic_cast<void*>(pb11_sta);
    void* const   pdyn20   = dynamic_cast<void*>(pb20_sta);
    void* const   pdyn21   = dynamic_cast<void*>(pb21_sta);
    void* const   pdyn     = dynamic_cast<void*>(&d);
    return
      eq (pdyn00, &d) &&
      eq (pdyn10, &d) &&
      eq (pdyn11, &d) &&
      eq (pdyn20, &d) &&
      eq (pdyn21, &d) &&
      eq (pdyn, &d);
  }

//==========
// down-cast
//----------
  bool test_v0102()
  {
    derived d;

    base00* const  pb00_sta = &d;
    base20* const  pb20_dyn = dynamic_cast<base20*>(pb00_sta);

    base11* const  pb11_sta = &d;
    derived* const pd_dyn   = dynamic_cast<derived*>(pb11_sta);

    return
      eq (pb20_dyn, static_cast<base20*>(&d)) &&
      eq (pd_dyn, &d);
  }

//======================
// left-right cross-cast
//----------------------
  bool test_v0103()
  {
    derived d;
    base20* const pb20_sta = &d;
    base11* const pb11_sta = &d;
    base11* const pb11_dyn = dynamic_cast<base11*>(pb20_sta);
    return
      eq (pb11_dyn, pb11_sta);
  }

//======================
// right-left cross-cast
//----------------------
  bool test_v0104()
  {
    derived d;
    base11* const pb11_sta = &d;
    base10* const pb10_sta = &d;
    base10* const pb10_dyn = dynamic_cast<base10*>(pb11_sta);
    return
      eq (pb10_dyn, pb10_sta);
  }

}}}  // namespace cpprtl::rtti::test

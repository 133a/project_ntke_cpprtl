//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// virtual inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00 { int a; virtual ~base00() {} };

  class base10 : virtual private base00 { int a; };
  class base11 :         public  base00 { int a; int b; };

  class derived : public base10, public base11 { int a; };

#if 0
  base00   base00
    v-       |
    v-       |
  base10   base11
      \     /
       \   /
        \ /
      derived
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_v0401()
  {
    derived d;
    base10* const pb10_sta  = &d;
    base11* const pb11_sta  = &d;
    base00* const pb00_sta0 = (base00*)pb10_sta;
    base00* const pb00_sta1 = pb11_sta;
    void* const   pdyn10    = dynamic_cast<void*>(pb10_sta);
    void* const   pdyn11    = dynamic_cast<void*>(pb11_sta);
    void* const   pdyn000   = dynamic_cast<void*>(pb00_sta0);
    void* const   pdyn001   = dynamic_cast<void*>(pb00_sta1);
    void* const   pdyn      = dynamic_cast<void*>(&d);
    return
      eq (pdyn10, &d) &&
      eq (pdyn11, &d) &&
      eq (pdyn000, &d) &&
      eq (pdyn001, &d) &&
      eq (pdyn, &d);
  }

//==========
// down-cast
//----------
  bool test_v0402()
  {
    derived d;
    base00* const  pb00_sta = static_cast<base11*>(&d);
    derived* const pd_dyn   = dynamic_cast<derived*>(pb00_sta);
    return
      eq (pd_dyn, &d);
  }

//==============
// wrong up-cast
//--------------
  bool test_v0403()
  {
    derived d;
    base00* const pb00_dyn = dynamic_cast<base00*>(&d);  // msvc warning C4540
    return
      eq (pb00_dyn, 0);
  }

//======================
// left-right cross-cast
//----------------------
  bool test_v0404()
  {
    derived d;
    base10* const pb10_sta = &d;
    base00* const pb00_dyn = dynamic_cast<base00*>(pb10_sta);  // msvc warning C4540
    base11* const pb11_dyn = dynamic_cast<base11*>(pb10_sta);
    return
      eq (pb00_dyn, 0) &&
      eq (pb11_dyn, static_cast<base11*>(&d));
  }

//======================
// right-left cross-cast
//----------------------
  bool test_v0405()
  {
    derived d;

    base00* const pb00_sta   = static_cast<base11*>(&d);
    base10* const pb10_dyn00 = dynamic_cast<base10*>(pb00_sta);

    base11* const pb11_sta   = &d;
    base10* const pb10_dyn11 = dynamic_cast<base10*>(pb11_sta);

    return
      eq (pb10_dyn00, static_cast<base10*>(&d)) &&
      eq (pb10_dyn11, static_cast<base10*>(&d));
  }

}}}  // namespace cpprtl::rtti::test

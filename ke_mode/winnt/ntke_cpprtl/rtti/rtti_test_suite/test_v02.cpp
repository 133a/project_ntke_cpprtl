//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// virtual private/protected inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00 { int a; virtual ~base00() {} };

  struct base10 : virtual private   base00 { int a; virtual ~base10() {} };
  struct base11 : virtual protected base00 { int a; int b; virtual ~base11() {} };
  struct base12 : virtual public    base00 { int a; int b; int c; virtual ~base12() {} };

  class derived : public base10, public base11, public base12 { int a; };

#if 0
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
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_v0201()
  {
    derived d;
    base00* const pb00_sta = &d;
    base10* const pb10_sta = &d;
    base11* const pb11_sta = &d;
    base12* const pb12_sta = &d;
    void* const   pdyn00   = dynamic_cast<void*>(pb00_sta);
    void* const   pdyn10   = dynamic_cast<void*>(pb10_sta);
    void* const   pdyn11   = dynamic_cast<void*>(pb11_sta);
    void* const   pdyn12   = dynamic_cast<void*>(pb12_sta);
    void* const   pdyn     = dynamic_cast<void*>(&d);
    return
      eq (pdyn00, &d) &&
      eq (pdyn10, &d) &&
      eq (pdyn11, &d) &&
      eq (pdyn12, &d) &&
      eq (pdyn, &d);
  }

//==========
// down-cast
//----------
  bool test_v0202()
  {
    derived d;

    base00* const  pb00_sta = &d;
    derived* const pd_dyn00 = dynamic_cast<derived*>(pb00_sta);
    base12* const  pb12_dyn = dynamic_cast<base12*>(pb00_sta);

    base10* const  pb10_sta = &d;
    derived* const pd_dyn10 = dynamic_cast<derived*>(pb10_sta);

    base11* const  pb11_sta = &d;
    derived* const pd_dyn11 = dynamic_cast<derived*>(pb11_sta);

    base12* const  pb12_sta = &d;
    derived* const pd_dyn12 = dynamic_cast<derived*>(pb12_sta);

    return
      eq (pd_dyn00, &d) &&
      eq (pb12_dyn, static_cast<base12*>(&d)) &&
      eq (pd_dyn10, &d) &&
      eq (pd_dyn11, &d) &&
      eq (pd_dyn12, &d);
  }

//======================
// left-right cross-cast
//----------------------
  bool test_v0203()
  {
    derived d;
    base10* const pb10_sta  = &d;
    base11* const pb11_dyn  = dynamic_cast<base11*>(pb10_sta);
    base12* const pb12_dyn0 = dynamic_cast<base12*>(pb10_sta);
    base12* const pb12_dyn1 = dynamic_cast<base12*>(pb11_dyn);
    return
      eq (pb11_dyn, static_cast<base11*>(&d)) &&
      eq (pb12_dyn0, static_cast<base12*>(&d)) &&
      eq (pb12_dyn1, static_cast<base12*>(&d));
  }

//======================    
// right-left cross-cast
//----------------------
  bool test_v0204()
  {
    derived d;
    base12* const pb12_sta  = &d;
    base11* const pb11_dyn  = dynamic_cast<base11*>(pb12_sta);
    base10* const pb10_dyn0 = dynamic_cast<base10*>(pb12_sta);
    base10* const pb10_dyn1 = dynamic_cast<base10*>(pb11_dyn);
    return
      eq (pb11_dyn, static_cast<base11*>(&d)) &&
      eq (pb10_dyn0, static_cast<base10*>(&d)) &&
      eq (pb10_dyn1, static_cast<base10*>(&d));
  }

//==================================================================
// cast to virtual base
// up-casts are not possible, while left-right cross-casts are valid
//------------------------------------------------------------------
  bool test_v0205()
  {
    derived d;

    base10* const pb10_sta = &d;
    base00* const pb00_dyn10 = dynamic_cast<base00*>(pb10_sta);  // msvc warning C4540

    base11* const pb11_sta = &d;
    base00* const pb00_dyn11 = dynamic_cast<base00*>(pb11_sta);  // msvc warning C4540

    return
      eq (pb00_dyn10, static_cast<base00*>(&d)) &&
      eq (pb00_dyn11, static_cast<base00*>(&d));
  }

}}}  // namespace cpprtl::rtti::test

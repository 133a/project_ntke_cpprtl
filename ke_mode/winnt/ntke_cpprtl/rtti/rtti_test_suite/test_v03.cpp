//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// virtual ambiguous inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00 { public : int a; virtual ~base00() {} };

  struct base10 : virtual public base00 { int a; virtual ~base10() {} };

  class base20 : public base10 { int a; };
  class base21 : public base10 { int a; int b; };

  class derived : public base20, public base21 { int a; };

#if 0
      base00
        /\
       /  \
      /    \
  base10   base10
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
  bool test_v0301()
  {
    derived d;
    base00* const pb00_sta  = &d;
    base20* const pb20_sta  = &d;
    base21* const pb21_sta  = &d;
    base10* const pb10_sta0 = pb20_sta;
    base10* const pb10_sta1 = pb21_sta;
    void* const   pdyn00    = dynamic_cast<void*>(pb00_sta);
    void* const   pdyn20    = dynamic_cast<void*>(pb20_sta);
    void* const   pdyn21    = dynamic_cast<void*>(pb21_sta);
    void* const   pdyn100   = dynamic_cast<void*>(pb10_sta0);
    void* const   pdyn101   = dynamic_cast<void*>(pb10_sta1);
    void* const   pdyn      = dynamic_cast<void*>(&d);
    return
      eq (pdyn00, &d) &&
      eq (pdyn20, &d) &&
      eq (pdyn21, &d) &&
      eq (pdyn100, &d) &&
      eq (pdyn101, &d) &&
      eq (pdyn, &d);
  }

//==========
// down-cast
//----------
  bool test_v0302()
  {
    derived d;

    base00* const  pb00_sta  = &d;
    base20* const  pb20_dyn  = dynamic_cast<base20*>(pb00_sta);
    base21* const  pb21_dyn  = dynamic_cast<base21*>(pb00_sta);

    base10* const  pb10_sta0 = static_cast<base20*>(&d);
    derived* const pd_dyn0   = dynamic_cast<derived*>(pb10_sta0);

    base10* const  pb10_sta1 = static_cast<base21*>(&d);
    derived* const pd_dyn1   = dynamic_cast<derived*>(pb10_sta1);

    return
      eq (pb20_dyn, static_cast<base20*>(&d)) &&
      eq (pb21_dyn, static_cast<base21*>(&d)) &&
      eq (pd_dyn0, &d) &&
      eq (pd_dyn1, &d);
  }

//================
// wrong down-cast
//----------------
  bool test_v0303()
  {
    derived d;
    base00* const pb00_sta = &d;
    base10* const pb10_dyn = dynamic_cast<base10*>(pb00_sta);
    return
      eq (pb10_dyn, 0);
  }

//==============
// wrong up-cast
//--------------
  bool test_v0304()
  {
    derived d;
    base10* const pb10_dyn = dynamic_cast<base10*>(&d);  // msvc warning C4540
    return
      eq (pb10_dyn, 0);
  }

//======================
// left-right cross-cast
//----------------------
  bool test_v0305()
  {
    derived d;
    base10* const pb10_sta = static_cast<base20*>(&d);
    base21* const pb21_dyn = dynamic_cast<base21*>(pb10_sta);
    return
      eq (pb21_dyn, static_cast<base21*>(&d));
  }

//======================    
// right-left cross-cast
//----------------------
  bool test_v0306()
  {
    derived d;
    base10* const pb10_sta = static_cast<base21*>(&d);
    base20* const pb20_dyn = dynamic_cast<base20*>(pb10_sta);
    return
      eq (pb20_dyn, static_cast<base20*>(&d));
  }

}}}  // namespace cpprtl::rtti::test

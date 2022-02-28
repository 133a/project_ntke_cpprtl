//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// single inheritance tests
// MT-safe
#include "test_aux.h"


namespace
{
  struct base00
  {
    int a; int b; int c;
    virtual ~base00() {}
  };
  struct base0 : protected base00
  {
    int a; int b;
    virtual ~base0() {}
  };
  class base1 : public base0 { int a; int b; };
  class base2 : public base1 { int a; };
  class derived : public base2 { int a; int b; };

#if 0
  base00
    +
  base0
    |
  base1
    |
  base2
    |
  derived
#endif  // 0
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_s0101()
  {
    base1 b1;
    derived d;

    base1* const   pb1_sta1 = &b1;
    void* const    pdyn1    = dynamic_cast<void*>(pb1_sta1);

    base0* const   pb0_sta2 = &b1;
    void* const    pdyn2    = dynamic_cast<void*>(pb0_sta2);

    derived* const pd_sta3  = &d;
    void* const    pdyn3    = dynamic_cast<void*>(pd_sta3);

    base0* const   pb0_sta4 = &d;
    void* const    pdyn4    = dynamic_cast<void*>(pb0_sta4);

    return
      eq (pdyn1, &b1) &&
      eq (pdyn2, &b1) &&
      eq (pdyn3, &d)  &&
      eq (pdyn4, &d);
  }

//===================================
// compile-time resolved dynamic_cast
//-----------------------------------
  bool test_s0102()
  {
    derived d;
    base1* const pb_sta = &d;
    base1* const pb_dyn = dynamic_cast<base1*>(&d);
    return
      eq (pb_sta, pb_dyn);
  }

//===============================
// run-time resolved dynamic_cast
//-------------------------------
  bool test_s0103()
  {
    derived d;

    base0* const   pb0_sta = &d;
    derived* const pd_dyn  = dynamic_cast<derived*>(pb0_sta);

    base1&         rb1_sta = d;
    derived&       rd_dyn  = dynamic_cast<derived&>(rb1_sta);

    return
      eq (pd_dyn,  &d) &&
      eq (&rd_dyn, &d);
  }

//=============================
// run-time failed dynamic_cast
//-----------------------------
  bool test_s0104()
  {
    base1 b;
    base1* const   pb_sta = &b;
    derived* const pd_dyn = dynamic_cast<derived*>(pb_sta);
    return
      eq (pd_dyn, 0);
  }

//=========
// bad_cast
//---------
  bool test_s0105()
  try
  {
    base1 b;
    base1&   rb_sta = b;
    derived& rd_dyn = dynamic_cast<derived&>(rb_sta);
    return (void)rd_dyn, false;
  }
  catch (std::bad_cast&)
  {
    return true;
  }
  catch (...)
  {
    return false;
  }

//===================================
// dynamic_cast from not-visible base
//-----------------------------------
  bool test_s0106()
  {
    derived d;
    base00* const  pb00_sta = (base00*)&d;
    derived* const pd_dyn   = dynamic_cast<derived*>(pb00_sta);
    return
      eq (pd_dyn, 0);
  }

//============================================================================================================
// dynamic_cast to not-visible base
// msvc warning C4540: dynamic_cast used to convert to inaccessible or ambiguous base; run-time test will fail
//------------------------------------------------------------------------------------------------------------
  bool test_s0107()
  {
    derived d;
    base00* const pb00_dyn = dynamic_cast<base00*>(&d);
    return
      eq (pb00_dyn, 0);
  }

//==========
// type_info
//----------
  bool test_s0108()
  {
    base1 b1;
    derived d;
    base1* const   pb_sta0 = &b1;
    base1* const   pb_sta1 = &d;
    derived* const pd_dyn  = dynamic_cast<derived*>(pb_sta1);

    return
      typeid(*pd_dyn) != typeid(*pb_sta0) &&
      typeid(*pd_dyn) == typeid(*pb_sta1);
  }

}}}  // namespace cpprtl::rtti::test

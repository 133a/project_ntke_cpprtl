//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// MT-UNsafe, run once
#include "test.h"


namespace cpprtl { namespace gstatic { namespace test
{
//======================================
// global/static BIT-object runtime init
//--------------------------------------
  int init0101() { return 101; }
  int init0102();

  extern int const var0101;
  namespace
  {
    int const var0102 = init0102();
  }

  bool test_01()
  {
    return 101+102 == var0101+var0102;
  }

//======================================
// global/static UDT-object runtime init
//--------------------------------------
  extern some_type const var0201;
  namespace
  {
    some_type const var0202 = 202;
  }

  bool test_02()
  {
    return 201+202 == var0201+var0202;
  }

//==============================================
// function scope static UDT-object runtime init
//----------------------------------------------
  bool test_03()
  {
    static some_type var0301 = 301;
    static some_type var0302 = 302;
    return 301+302 == var0301+var0302;
  }
}}}  // namespace cpprtl::gstatic::test

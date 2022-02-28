//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifdef NT_KERNEL_MODE
#  include "ntddk.include.h"
#endif
#include "test.h"


namespace cpprtl { namespace gstatic { namespace test
{
//========
// test_01
//--------
  int init0101();
  int init0102() { return 102; }
  extern int const var0101 = init0101();


//==================
// test_02 / test_03
//------------------
  some_type::some_type(int i)
    : val ( i )
  {}

  some_type::~some_type()
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("~some_type()\n");
  #endif
  }

  some_type::operator int() const
  {
    return val;
  }

  extern some_type const var0201 = 201;
}}}  // namespace cpprtl::gstatic::test

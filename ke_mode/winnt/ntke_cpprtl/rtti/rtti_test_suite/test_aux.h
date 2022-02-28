//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef TEST_AUX_H_
#define TEST_AUX_H_


#include <typeinfo>


#ifdef _MSC_VER
  #define MSVC_RTTI_V0 0
  #define MSVC_RTTI_V1 1
  #define MSVC_RTTI_V2 2
  #define MSVC_RTTI_V3 3

  #if (_MSC_VER < 1310)
    #define MSVC_RTTI MSVC_RTTI_V0  // msvc2002/ddk2600
  #elif (_MSC_FULL_VER < 140050320)
    #define MSVC_RTTI MSVC_RTTI_V1  // msvc2003/ddk3790
  #elif (_MSC_FULL_VER < 190024210)
    #define MSVC_RTTI MSVC_RTTI_V2  // msvc2005-msvc2015u2, BC_WITH_HIERARCHY appeared
  #else
    #define MSVC_RTTI MSVC_RTTI_V3  // msvc2015u3 (19.00.24210) and newer
  #endif
#endif  // _MSC_VER


namespace cpprtl { namespace rtti { namespace test
{
  inline bool eq(void const* const p1, void const* const p2)
  {
    return p1 == p2;
  }
}}}

#endif  // include guard

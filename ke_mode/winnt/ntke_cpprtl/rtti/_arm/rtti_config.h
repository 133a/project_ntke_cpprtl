//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTTI_CONFIG_H_
#define RTTI_CONFIG_H_


#ifdef CPPRTL_CFG_RVA
#undef CPPRTL_CFG_RVA
#endif
#ifdef CPPRTL_RTTI_CFG_RVA
#undef CPPRTL_RTTI_CFG_RVA
#endif

namespace cpprtl
{
  typedef int           rva_t;
  typedef unsigned long imagebase_t;
}

#endif  // include guard

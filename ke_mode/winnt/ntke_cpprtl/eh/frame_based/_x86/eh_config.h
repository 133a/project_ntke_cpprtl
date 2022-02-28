//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_CONFIG_FRAME_BASED_H_
#define EH_CONFIG_FRAME_BASED_H_


#define CPPRTL_EH_CFG_FRAMEBASED

#ifdef CPPRTL_CFG_RVA
#undef CPPRTL_CFG_RVA
#endif
#ifdef CPPRTL_EH_CFG_RVA
#undef CPPRTL_EH_CFG_RVA
#endif

#if (_MSC_VER >= 1310) && (NTDDI_VERSION >= 0x05010000)  // msvc>=msvc2003/ddk3790 && nt>=nt5.1
#define CPPRTL_EH_CFG_SAFESEH
#endif

namespace cpprtl
{
  typedef ::size_t size_t;
  typedef unsigned rva_t;  // to handle SafeSEH
  typedef unsigned imagebase_t;
}


#endif  // include guard

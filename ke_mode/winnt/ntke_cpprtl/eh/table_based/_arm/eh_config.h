//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_CONFIG_H_
#define EH_CONFIG_H_


#define CPPRTL_CFG_RVA
#define CPPRTL_EH_CFG_RVA
#define CPPRTL_EH_CFG_TABLEBASED

namespace cpprtl
{
  typedef ::size_t      size_t;
  typedef int           rva_t;
#ifdef NT_KERNEL_MODE
  typedef unsigned int  imagebase_t;
#else
  typedef unsigned long imagebase_t;
#endif

  typedef   signed char int8_t;    // to feed up msvc_data_4
  typedef unsigned char uint8_t;   // ^
  typedef int           int32_t;   // ^
  typedef unsigned      uint32_t;  // ^
}

#endif  // include guard

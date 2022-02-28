//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_FRAMEWORK_KM_DDK_H_
#define EH_FRAMEWORK_KM_DDK_H_


#if (DDK_VER < 6000)
  extern "C"
  {
    #include <ntddk.h>
  #ifdef CPPRTL_EH_CFG_SAFESEH
    #include <ntimage.h>
  #endif
  }
#else
    #include <ntddk.h>
  #ifdef CPPRTL_EH_CFG_SAFESEH
    #include <ntimage.h>
  #endif
#endif


#define EXCEPTION_UNWINDING      0x2
#define EXCEPTION_TARGET_UNWIND  0x20
#ifndef EXCEPTION_UNWIND
#define EXCEPTION_UNWIND         0x66
#endif


#endif  // include guard

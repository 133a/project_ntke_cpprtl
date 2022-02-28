//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTTI_FRAMEWORK_H_
#define RTTI_FRAMEWORK_H_


#ifdef NT_KERNEL_MODE
  #if (DDK_VER < 6000)
    extern "C"
    {
      #include <ntddk.h>
    }
  #else
      #include <ntddk.h>
  #endif

  extern "C"
  {
    PVOID RtlPcToFileHeader
    (
      PVOID  PcValue
    , PVOID* BaseOfImage
    );
  }
#else
  #include <windows.h>

  extern "C"
  {
    __declspec(dllimport)
    PVOID RtlPcToFileHeader
    (
      PVOID  PcValue
    , PVOID* BaseOfImage
    );
  }
#endif  // NT_KERNEL_MODE


#include "rtti_config.h"
#include <check_irql.h>


#endif  // include guard

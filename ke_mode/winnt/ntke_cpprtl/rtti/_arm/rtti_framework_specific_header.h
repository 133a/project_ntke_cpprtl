/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_FRAMEWORK_SPECIFIC_HEADER_H_
#define RTTI_FRAMEWORK_SPECIFIC_HEADER_H_


#ifdef NT_KERNEL_MODE

  #if DDK_VER < 9000
  #  error unsupported toolset
  #else
  #  include <ntddk.h>
  #endif


  extern "C"
  {
    void* RtlPcToFileHeader
    (
      void  *   PcValue
    , void  **  BaseOfImage
    );
  }

#else
#  include <windows.h>
#endif  // NT_KERNEL_MODE


#include "rtti_config.h"
#include <check_irql.h>


#endif  // include guard


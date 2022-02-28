//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTL_FRAMEWORK_H_
#define RTL_FRAMEWORK_H_


#ifndef NOMINMAX
#define NOMINMAX 1
#endif


#if defined (NT_KERNEL_MODE)

  #if DDK_VER < 6000
    extern "C"
    {
    #ifdef DRIVER_TYPE_WDM
      #include <wdm.h>
    #else
      #include <ntddk.h>
    #endif
    }
  #else
    #ifdef DRIVER_TYPE_WDM
      #include <wdm.h>
    #else
      #include <ntddk.h>
    #endif
  #endif

#elif defined (NT_NATIVE_MODE)
  // TODO
#else  // (um)
  #include <windows.h>
#endif  // NT_KERNEL_MODE


#include "check_irql.h"


#endif  // include guard


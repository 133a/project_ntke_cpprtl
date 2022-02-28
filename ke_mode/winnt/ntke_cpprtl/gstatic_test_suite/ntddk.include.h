//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef NTDDK_INCLUDE_H_
#define NTDDK_INCLUDE_H_


#if DDK_VER < 6000
  extern "C"
  {
    #include <ntddk.h>
  }
#else
    #include <ntddk.h>
#endif


#endif  // include guard

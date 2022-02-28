//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTTI_FRAMEWORK_H_
#define RTTI_FRAMEWORK_H_


#ifdef NT_KERNEL_MODE
  #include <ntddk.h>
#else
  #include <windows.h>
#endif  // NT_KERNEL_MODE


#include "rtti_config.h"
#include <check_irql.h>


#endif  // include guard

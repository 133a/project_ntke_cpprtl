//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_FRAMEWORK_H_
#define EH_FRAMEWORK_H_


#ifdef NT_KERNEL_MODE
#  include "eh_framework_km_ddk.h"
#else
#  include "eh_framework_um_sdk.h"
#endif


#include "eh_config.h"
#include <check_irql.h>


#endif  // include guard


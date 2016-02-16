/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_FRAMEWORK_SPECIFIC_HEADER_H_
#define EH_FRAMEWORK_SPECIFIC_HEADER_H_


#ifdef NT_KERNEL_MODE
#  include "eh_km_ddk.h"
#else
#  include "eh_um_sdk.h"
#endif


#include "eh_config.h"
#include <check_irql.h>


#endif  //  include guard


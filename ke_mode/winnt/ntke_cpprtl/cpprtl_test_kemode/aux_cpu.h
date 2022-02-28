//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef AUX_CPU_H_
#define AUX_CPU_H_


#include "ntddk.include.h"


namespace aux_
{
  ULONG get_number_processors();
  ULONG get_current_processor();
}


#endif  // include guard


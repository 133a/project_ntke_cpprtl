/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef AUX_CPU_H_
#define AUX_CPU_H_


#include "ntddk.include.h"


namespace aux_
{
  ULONG get_number_processors();
  ULONG get_current_processor();
}


#endif  // include guard


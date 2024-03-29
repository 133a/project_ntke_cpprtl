//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "ntddk.include.h"
#include "aux_cpu.h"


namespace aux_
{

  ULONG get_number_processors()
  {
  #if (NTDDI_VERSION >= 0x05010000)  // >=nt5.1
    return KeNumberProcessors;
  #else
    return *KeNumberProcessors;
  #endif
  }


  ULONG get_current_processor()
  {
  #if (NTDDI_VERSION >= 0x06010000)  // >=nt6.1
    return KeGetCurrentProcessorNumberEx(0);  // any IRQL
  #else
    ASSERT ( KeGetCurrentIrql() >= DISPATCH_LEVEL );  // recommended
    return KeGetCurrentProcessorNumber();
  #endif
  }

}


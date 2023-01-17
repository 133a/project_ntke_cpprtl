;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


#include <ksarm.h>


  ; r0 = nvcontext const*
  LEAF_ENTRY_NO_PDATA cpprtl_eh_start_context

    vldm  r0! , {d8-d15}
    ldm   r0! , {r4-r11}
    ldr   sp  , [r0]
    ldr   lr  , [r0, #4]  ; load lr from ctx.pc
    bx    lr

  LEAF_END_NO_PDATA

  END

;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


#include <ksarm.h>

;=====================================;
; cpprtl_rtti_get_co_descriptor stuff ;
;-------------------------------------;

  ; r0 = EXCEPTION_RECORD*
  ; r1 = void* frame
  ; r2 = CONTEXT*
  ; r3 = DISPATCHER_CONTEXT*
  NESTED_ENTRY get_co_dsc_handler
    PROLOG_PUSH {r4-r6, lr}
    mov    r4 , #1  ; ExceptionContinueSearch
    mov32  r5 , #STATUS_ACCESS_VIOLATION
    ldr    r6 , [r0, #ErExceptionCode]
    cmp    r5 , r6
    bne    %F0
    ldr    r5 , =exit
    str    r5 , [r2, #CxPc]
    mov    r4 , #0  ; ExceptionContinueExecution
    str    r4 , [r2, #CxR0]  ; reset &co_dsc
0
    mov    r0 , r4
    EPILOG_POP {r4-r6, pc}
    ; frame disposition
  NESTED_END

  ; r0 = &obj
  NESTED_ENTRY cpprtl_rtti_get_co_descriptor, , get_co_dsc_handler
    ldr  r1 , [r0]       ; AV
    ldr  r0 , [r1, #-4]  ; ^ r0=&co_dsc
    ldr  r1 , [r0]       ; ^ probe co_dsc.signature
exit
    EPILOG_RETURN
    ; &co_dsc
  NESTED_END


  END

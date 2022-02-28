;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


#include <ksarm.h>


;====================================;
; cpprtl_eh_run_unwind_funclet stuff ;
;------------------------------------;

  import cpprtl_eh_unwind_handler

  ; r0 = object_type*     - dtor argument, passthrough to the funclet
  ; r1 = function_frame_t - function frame
  ; r2 = funclet_ft       - entry point
  ; r3 = size_t[]         - {r4-r11} the funclet nv-context
  NESTED_ENTRY cpprtl_eh_run_unwind_funclet, , cpprtl_eh_unwind_handler
    PROLOG_PUSH {r1, r4-r11, lr}  ; TODO is it necessary to keep r1 at the stack top for unwind funclets?
    ldm  r3 , {r4-r11} ; load the funclet {r4-r11} nv-context
    blx  r2            ; run the funclet
    nop                ; required for unwind
    EPILOG_POP  {r3-r11, pc}
  NESTED_END


;=================================;
; cpprtl_eh_run_catch_block stuff ;
;---------------------------------;

  import cpprtl_eh_catch_block_handler

  ;  r1   dword  0x00  function_frame_t <--sp
  ;  r2   dword  0x04  &exc_rec
  ;  r3   dword  0x08  _aligner_
  ;  r4   dword  0x0C  NV
  ;  r5   dword  0x10  ^
  ;  r6   dword  0x14  ^
  ;  r7   dword  0x18  ^
  ;  r8   dword  0x1C  ^
  ;  r9   dword  0x20  ^
  ;  r10  dword  0x24  ^
  ;  r11  dword  0x28  ^
  ;  lr   dword  0x2C  ^
  ;              0x30  <--frame base
  #define exc_rec_top_offset   0x04
  #define exc_rec_base_offset -0x2C

  ; r0 = EXCEPTION_RECORD*   - the exception record
  ; r1 = void*               - cpprtl_run_catch_block frame base
  ; r2 = CONTEXT*
  ; r3 = DISPATCHER_CONTEXT*
  NESTED_ENTRY catch_block_handler_thunk
    PROLOG_PUSH {r4, lr}
    ldr  r1 , [r1, #exc_rec_base_offset]  ; fwd the previous exception record
    blx  cpprtl_eh_catch_block_handler    ; run the handler main payload
    nop                                   ; required for unwind
    EPILOG_POP  {r4, pc}
  NESTED_END

  ; r0 = funclet_ft        - catch block entry point
  ; r1 = function_frame_t  - function frame, keep at the stack top to make available while catch block unwind
  ; r2 = EXCEPTION_RECORD* - the current exception record, fwd to the frame handler
  ; r3 = size_t[]          - {r4-r11} the funclet nv-context
  NESTED_ENTRY cpprtl_eh_run_catch_block, , catch_block_handler_thunk
    PROLOG_PUSH {r1-r11, lr}
    ldm  r3 , {r4-r11}        ; load the funclet {r4-r11} nv-context
    blx  r0                   ; run the catch block, returns r0=continuation
    nop                       ; required for unwind
    EPILOG_POP  {r1-r11, pc}
  NESTED_END


;==========================;
; cpprtl_eh_run_dtor stuff ;
;--------------------------;

  import cpprtl_eh_dtor_handler

  ; r0 = object_type* - this pointer, passthrough to dtor
  ; r1 = dtor_ft      - destructor
  NESTED_ENTRY cpprtl_eh_run_dtor, , cpprtl_eh_dtor_handler
    PROLOG_PUSH {r4, lr}  ; save r4 as aligner
    blx  r1               ; run the dtor
    nop                   ; required for unwind
    EPILOG_POP  {r4, pc}
  NESTED_END


;====================================================;
; cpprtl_eh_run_cctor / cpprtl_eh_run_cctor_vb stuff ;
;----------------------------------------------------;

  import cpprtl_eh_cctor_handler

  ; r0 = object_type* - this pointer, passthrough to cctor
  ; r1 = object_type* - that pointer, passthrough to cctor
  ; r2 = cctor_ft     - copy constructor
  NESTED_ENTRY cpprtl_eh_run_cctor, , cpprtl_eh_cctor_handler
    PROLOG_PUSH {r4, lr}  ; save r4 as aligner
    blx  r2               ; run the cctor
    nop                   ; required for unwind
    EPILOG_POP  {r4, pc}
  NESTED_END

  ; r0 = object_type* - this pointer, passthrough to cctor_vb
  ; r1 = object_type* - that pointer, passthrough to cctor_vb
  ; r2 = size_t       - passthrough to cctor_vb
  ; r3 = cctor_vb_ft  - copy constructor for classes having virtual bases
  NESTED_ENTRY cpprtl_eh_run_cctor_vb, , cpprtl_eh_cctor_handler
    PROLOG_PUSH {r4, lr}  ; save r4 as aligner
    blx  r3               ; run the cctor_vb
    nop                   ; required for unwind
    EPILOG_POP  {r4, pc}
  NESTED_END


  END

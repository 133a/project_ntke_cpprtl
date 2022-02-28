;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


include ksamd64.inc


.code

;==============================================================;
; cpprtl_eh_run_unwind_funclet/cpprtl_eh_run_unwind_dtor stuff ;
;--------------------------------------------------------------;

extern cpprtl_eh_unwind_handler : proc

run_funclet_frame struct
  RCX_shadow  qword ?
  RDX_shadow  qword ?
  R8_shadow   qword ?
  R9_shadow   qword ?
  _aligner_   qword ?
run_funclet_frame ends

; rcx = object_type*     - dtor argument, pass to the funclet
; rdx = function_frame_t - function frame, pass to the funclet
; r8  = funclet_ft       - entry point
NESTED_ENTRY cpprtl_eh_run_unwind_funclet, _TEXT, cpprtl_eh_unwind_handler
  alloc_stack(sizeof run_funclet_frame)
  END_PROLOGUE
  call  r8
  nop  ; required for unwind
  add   rsp , sizeof run_funclet_frame
  ret  ; nothing is expected
NESTED_END cpprtl_eh_run_unwind_funclet, _TEXT


;=================================;
; cpprtl_eh_run_catch_block stuff ;
;---------------------------------;

extern cpprtl_eh_catch_block_handler : proc

run_catch_block_frame struct
  RCX_shadow  qword ?
  RDX_shadow  qword ?
  R8_shadow   qword ?
  R9_shadow   qword ?
  exc_rec     qword ?
run_catch_block_frame ends

catch_block_handler_frame struct
  RCX_shadow  qword ?
  RDX_shadow  qword ?
  R8_shadow   qword ?
  R9_shadow   qword ?
  _aligner_   qword ?
catch_block_handler_frame ends

; rcx = void* new_exc
; rdx = void* run_catch_block_frame
; r8  = CONTEXT*
; r9  = DISPATCHER_CONTEXT*
NESTED_ENTRY catch_block_handler_thunk, _TEXT
  alloc_stack(sizeof catch_block_handler_frame)
  END_PROLOGUE
  mov   rdx , [rdx].run_catch_block_frame.exc_rec
  call  cpprtl_eh_catch_block_handler
  nop
  add   rsp , sizeof catch_block_handler_frame
  ret
NESTED_END catch_block_handler_thunk, _TEXT

; rcx = funclet_ft        - catch block entry point
; rdx = function_frame_t  - function frame, pass to the funclet
; r8  = EXCEPTION_RECORD* - the current exception record, fwd to the frame handler
NESTED_ENTRY cpprtl_eh_run_catch_block, _TEXT, catch_block_handler_thunk
  alloc_stack(sizeof run_catch_block_frame)
  END_PROLOGUE
  mov   [rsp].run_catch_block_frame.exc_rec , r8
  call  rcx
  nop  ; required for unwind
  add   rsp , sizeof run_catch_block_frame
  ret  ; rax = the continuation address
NESTED_END cpprtl_eh_run_catch_block, _TEXT


;==========================;
; cpprtl_eh_run_dtor stuff ;
;--------------------------;

extern cpprtl_eh_dtor_handler : proc

; rcx = object_type* - this pointer, pass to dtor
; rdx = dtor_ft - destructor
NESTED_ENTRY cpprtl_eh_run_dtor, _TEXT, cpprtl_eh_dtor_handler
  alloc_stack(sizeof run_funclet_frame)
  END_PROLOGUE
  call  rdx
  nop  ; required for unwind
  add   rsp , sizeof run_funclet_frame
  ret
NESTED_END cpprtl_eh_run_dtor, _TEXT


;====================================================;
; cpprtl_eh_run_cctor / cpprtl_eh_run_cctor_vb stuff ;
;----------------------------------------------------;

extern cpprtl_eh_cctor_handler : proc

; rcx = object_type* - this pointer, pass to cctor
; rdx = object_type* - that pointer, pass to cctor
; r8  = cctor_ft - copy constructor
NESTED_ENTRY cpprtl_eh_run_cctor, _TEXT, cpprtl_eh_cctor_handler
  alloc_stack(sizeof run_funclet_frame)
  END_PROLOGUE
  call  r8
  nop  ; required for unwind
  add   rsp , sizeof run_funclet_frame
  ret
NESTED_END cpprtl_eh_run_cctor, _TEXT

; rcx = object_type* - this pointer, pass to cctor_vb
; rdx = object_type* - that pointer, pass to cctor_vb
; r8  = size_t - pass to cctor_vb
; r9  = cctor_vb_ft - copy constructor for classes having virtual bases
NESTED_ENTRY cpprtl_eh_run_cctor_vb, _TEXT, cpprtl_eh_cctor_handler
  alloc_stack(sizeof run_funclet_frame)
  END_PROLOGUE
  call  r9
  nop  ; required for unwind
  add   rsp , sizeof run_funclet_frame
  ret
NESTED_END cpprtl_eh_run_cctor_vb, _TEXT


END

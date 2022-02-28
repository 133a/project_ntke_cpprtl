;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


include ksamd64.inc

;=====================================;
; cpprtl_rtti_get_co_descriptor stuff ;
;-------------------------------------;

; CONTEXT offsets
CtxRax equ 078H
CtxRip equ 0F8H
; EXCEPTION_RECORD offsets
ExcCode  equ 00H
ExcFlags equ 04H

AV equ 0C0000005H

; rcx = EXCEPTION_RECORD*
; rdx = void* frame
; r8  = CONTEXT*
; r9  = DISPATCHER_CONTEXT*
LEAF_ENTRY get_co_dsc_handler, _TEXT, NoPad
  xor  eax , eax
  inc  eax ; ExceptionContinueSearch
  mov  edx , [rcx+ExcCode]
  cmp  edx , AV
  jne  @F
  mov  rax , offset exit
  mov  [r8+CtxRip] , rax
  xor  eax , eax  ; ExceptionContinueExecution
  mov  [r8+CtxRax] , rax  ; reset &co_dsc
@@:
  ret  ; frame disposition
LEAF_END get_co_dsc_handler, _TEXT

; rcx = &obj
NESTED_ENTRY cpprtl_rtti_get_co_descriptor, _TEXT, get_co_dsc_handler, NoPad
  alloc_stack 8
  END_PROLOGUE
  mov  rcx , [rcx]    ; AV
  mov  rax , [rcx-8]  ; ^ rax=&co_dsc
  mov  ecx , [rax]    ; ^ probe co_dsc.signature
exit::
  add  rsp , 8
  ret  ; &co_dsc
NESTED_END cpprtl_rtti_get_co_descriptor, _TEXT

END

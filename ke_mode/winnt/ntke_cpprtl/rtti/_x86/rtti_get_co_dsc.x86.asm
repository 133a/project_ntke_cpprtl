;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------

.386
.model flat
assume fs:nothing

;=====================================;
; cpprtl_rtti_get_co_descriptor stuff ;
;-------------------------------------;

; CONTEXT offsets
CtxEax equ 0B0H
CtxEip equ 0B8H
; EXCEPTION_RECORD offsets
ExcCode  equ 00H
ExcFlags equ 04H

AV equ 0C0000005H

.code

; cdecl
; [ebp+8]  = EXCEPTION_RECORD*
; [ebp+12] = void* frame
; [ebp+16] = CONTEXT*
; [ebp+20] = DISPATCHER_CONTEXT*
_get_co_dsc_handler proc
  push ebp
  mov  ebp , esp
  xor  eax , eax
  inc  eax ; ExceptionContinueSearch
  mov  ecx , [ebp+8]
  mov  edx , [ecx+ExcCode]
  cmp  edx , AV
  jne  @F
  mov  ecx , [ebp+16]
  mov  eax , offset exit
  mov  [ecx+CtxEip] , eax
  xor  eax , eax  ; ExceptionContinueExecution
  mov  [ecx+CtxEax] , eax  ; reset &co_dsc
@@:
  pop  ebp
  ret  ; frame disposition
_get_co_dsc_handler endp

; fastcall
; ecx = &obj
@cpprtl_rtti_get_co_descriptor@4 proc public
  push ebp
  mov  ebp , esp
  push offset _get_co_dsc_handler
  push dword ptr fs:[0]
  mov  fs:[0] , esp
  mov  ecx , [ecx]    ; AV
  mov  eax , [ecx-4]  ; ^ eax=&co_dsc
  mov  ecx , [eax]    ; ^ probe co_dsc.signature
exit::
  pop  dword ptr fs:[0]
  mov  esp , ebp
  pop  ebp
  ret  ; &co_dsc
@cpprtl_rtti_get_co_descriptor@4 endp


;========================;
; SafeSEH >= ml.x86 7.10 ;
;------------------------;
if @Version ge 710
.safeseh _get_co_dsc_handler
endif


END

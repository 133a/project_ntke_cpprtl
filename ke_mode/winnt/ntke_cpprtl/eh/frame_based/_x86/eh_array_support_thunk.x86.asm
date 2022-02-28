;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


.386
.model flat

; CPPRTL_EH_CFG_EHVEC_THUNK
; extern "C" __stdcall ehvec_...()
extern _ehvec_dtor_impl@16 : proc
extern _ehvec_ctor_impl@20 : proc
extern _ehvec_ctor_vb_impl@20 : proc
extern _ehvec_copy_ctor_impl@24 : proc
extern _ehvec_copy_ctor_vb_impl@24 : proc

.code

;=============================================================================;
; `eh vector destructor iterator'                                             ;
; void __stdcall __ehvec_dtor(void*, unsigned, int, void(__thiscall*)(void*)) ;
;-----------------------------------------------------------------------------;
??_M@YGXPAXIHP6EX0@Z@Z proc public
  jmp _ehvec_dtor_impl@16
??_M@YGXPAXIHP6EX0@Z@Z endp

;=======================================================================================================;
; `eh vector constructor iterator'                                                                      ;
; void __stdcall __ehvec_ctor(void*, unsigned, int, void(__thiscall*)(void*), void(__thiscall*)(void*)) ;
;-------------------------------------------------------------------------------------------------------;
??_L@YGXPAXIHP6EX0@Z1@Z proc public
  jmp _ehvec_ctor_impl@20
??_L@YGXPAXIHP6EX0@Z1@Z endp

;==========================================================================================================;
; `eh vector vbase constructor iterator'                                                                   ;
; void __stdcall __ehvec_ctor_vb(void*, unsigned, int, void(__thiscall*)(void*), void(__thiscall*)(void*)) ;
;----------------------------------------------------------------------------------------------------------;
??_N@YGXPAXIHP6EX0@Z1@Z proc public
  jmp _ehvec_ctor_vb_impl@20
??_N@YGXPAXIHP6EX0@Z1@Z endp

;===================================================================================================================;
; `eh vector copy constructor iterator'                                                                             ;
; void __stdcall __ehvec_copy_ctor(void*, unsigned, int, void(__thiscall*)(void*, void*), void(__thiscall*)(void*)) ;
;-------------------------------------------------------------------------------------------------------------------;
??__C@YGXPAX0IHP6EX00@ZP6EX0@Z@Z proc public
  jmp _ehvec_copy_ctor_impl@24
??__C@YGXPAX0IHP6EX00@ZP6EX0@Z@Z endp

;======================================================================================================================;
; `eh vector vbase copy constructor iterator'                                                                          ;
; void __stdcall __ehvec_copy_ctor_vb(void*, unsigned, int, void(__thiscall*)(void*, void*), void(__thiscall*)(void*)) ;
;----------------------------------------------------------------------------------------------------------------------;
??__D@YGXPAX0IHP6EX00@ZP6EX0@Z@Z proc public
  jmp _ehvec_copy_ctor_vb_impl@24
??__D@YGXPAX0IHP6EX00@ZP6EX0@Z@Z endp

END

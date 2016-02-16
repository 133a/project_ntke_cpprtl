;/////////////////////////////////////////////////////////////////////////////
;////    copyright (c) 2012-2016 project_ntke_cpprtl
;////    mailto:kt133a@seznam.cz
;////    license: the MIT license
;/////////////////////////////////////////////////////////////////////////////


.386
.model flat


; msvc entry points' mangled names
public ??_M@YGXPAXIHP6EX0@Z@Z             ; void __stdcall __ehvec_dtor         (void*, unsigned int, int, void (__thiscall *)(void*))
public ??_L@YGXPAXIHP6EX0@Z1@Z            ; void __stdcall __ehvec_ctor         (void*, unsigned int, int, void (__thiscall *)(void*), void (__thiscall *)(void*))
public ??_N@YGXPAXIHP6EX0@Z1@Z            ; void __stdcall __ehvec_ctor_vb      (void*, unsigned int, int, void (__thiscall *)(void*), void (__thiscall *)(void*))
public ??__C@YGXPAX0IHP6EX00@ZP6EX0@Z@Z   ; void __stdcall __ehvec_copy_ctor    (void*, unsigned int, int, void (__thiscall *)(void*, void*), void (__thiscall *)(void*))
public ??__D@YGXPAX0IHP6EX00@ZP6EX0@Z@Z   ; void __stdcall __ehvec_copy_ctor_vb (void*, unsigned int, int, void (__thiscall *)(void*, void*), void (__thiscall *)(void*))


; extern "C" __stdcall
extrn   _ehvec_dtor_impl@16:proc
extrn   _ehvec_ctor_impl@20:proc
extrn   _ehvec_ctor_vb_impl@20:proc
extrn   _ehvec_copy_ctor_impl@24:proc
extrn   _ehvec_copy_ctor_vb_impl@24:proc

ehvec_dtor       equ     _ehvec_dtor_impl@16
ehvec_ctor       equ     _ehvec_ctor_impl@20
ehvec_ctor_vb    equ     _ehvec_ctor_vb_impl@20
ehvec_cctor      equ     _ehvec_copy_ctor_impl@24
ehvec_cctor_vb   equ     _ehvec_copy_ctor_vb_impl@24


.code


;**************************************
; eh vector destructor iterator
;**************************************
; void __stdcall __ehvec_dtor(void*, unsigned int, int, void (__thiscall *)(void*))
;**************************************
??_M@YGXPAXIHP6EX0@Z@Z  Proc
  jmp   ehvec_dtor
??_M@YGXPAXIHP6EX0@Z@Z  EndP


;***************************************
; eh vector constructor iterator
;***************************************
; void __stdcall __ehvec_ctor(void*, unsigned int, int, void (__thiscall *)(void*), void (__thiscall *)(void*))
;***************************************
??_L@YGXPAXIHP6EX0@Z1@Z  Proc
  jmp   ehvec_ctor
??_L@YGXPAXIHP6EX0@Z1@Z  EndP


;***************************************
; eh vector vbase constructor iterator
;***************************************
; void __stdcall __ehvec_ctor_vb(void*, unsigned int, int, void (__thiscall *)(void*), void (__thiscall *)(void*))
;***************************************
??_N@YGXPAXIHP6EX0@Z1@Z  Proc
  jmp   ehvec_ctor_vb
??_N@YGXPAXIHP6EX0@Z1@Z  EndP


;***************************************
; eh vector copy constructor iterator
;***************************************
; void __stdcall __ehvec_copy_ctor(void*, unsigned int, int, void (__thiscall *)(void*, void*), void (__thiscall *)(void*))
;***************************************
??__C@YGXPAX0IHP6EX00@ZP6EX0@Z@Z  Proc
  jmp   ehvec_cctor
??__C@YGXPAX0IHP6EX00@ZP6EX0@Z@Z  EndP


;***************************************
; eh vector vbase copy constructor iterator
;***************************************
; void __stdcall __ehvec_copy_ctor_vb (void*, unsigned int, int, void (__thiscall *)(void*, void*), void (__thiscall *)(void*))
;***************************************
??__D@YGXPAX0IHP6EX00@ZP6EX0@Z@Z  Proc
  jmp   ehvec_cctor_vb
??__D@YGXPAX0IHP6EX00@ZP6EX0@Z@Z  EndP


End


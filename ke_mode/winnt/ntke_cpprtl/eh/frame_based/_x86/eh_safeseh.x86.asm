;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------

.386
.model flat

public __except_list
__except_list equ 0

.code

;==============;
; dtor handler ;
;--------------;
extern _cpprtl_eh_dtor_terminator : proc

_safeseh_dtor_handler proc public
jmp _cpprtl_eh_dtor_terminator
_safeseh_dtor_handler endp


;===============;
; cctor handler ;
;---------------;
extern _cpprtl_eh_cctor_terminator : proc

_safeseh_cctor_handler proc public
jmp _cpprtl_eh_cctor_terminator
_safeseh_cctor_handler endp


;========================;
; unwind funclet handler ;
;------------------------;
extern _cpprtl_eh_funclet_terminator : proc

_safeseh_funclet_handler proc public
jmp _cpprtl_eh_funclet_terminator
_safeseh_funclet_handler endp


;=====================;
; catch block handler ;
;---------------------;
extern _cpprtl_eh_catch_block_handler : proc

_safeseh_catch_block_handler proc public
jmp _cpprtl_eh_catch_block_handler
_safeseh_catch_block_handler endp


;========================;
; SafeSEH >= ml.x86 7.10 ;
;------------------------;
if @Version ge 710
.safeseh _safeseh_dtor_handler
.safeseh _safeseh_cctor_handler
.safeseh _safeseh_funclet_handler
.safeseh _safeseh_catch_block_handler
endif
       
END

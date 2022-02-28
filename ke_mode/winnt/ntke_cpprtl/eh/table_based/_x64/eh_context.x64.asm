;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


m128 struct  ; 'xmmword' or 'oword' are unsupported by ddk3790-ml64
  qw0  qword ?
  qw1  qword ?
m128 ends

nvcontext struct  ; cpprtl::eh::x64::nvcontext look-alike
  _xmm6   m128 <>
  _xmm7   m128 <>
  _xmm8   m128 <>
  _xmm9   m128 <>
  _xmm10  m128 <>
  _xmm11  m128 <>
  _xmm12  m128 <>
  _xmm13  m128 <>
  _xmm14  m128 <>
  _xmm15  m128 <>
  _rip    qword ?
  _rbx    qword ?
  _rbp    qword ?
  _rsi    qword ?
  _rdi    qword ?
  _rsp    qword ?
  _r12    qword ?
  _r13    qword ?
  _r14    qword ?
  _r15    qword ?
nvcontext ends


.code

; rcx = nvcontext const*
cpprtl_eh_start_context proc public

  movdqa  xmm6  , [rcx].nvcontext._xmm6   ; restore XMM-registers
  movdqa  xmm7  , [rcx].nvcontext._xmm7   ; ^
  movdqa  xmm8  , [rcx].nvcontext._xmm8   ; ^
  movdqa  xmm9  , [rcx].nvcontext._xmm9   ; ^
  movdqa  xmm10 , [rcx].nvcontext._xmm10  ; ^
  movdqa  xmm11 , [rcx].nvcontext._xmm11  ; ^
  movdqa  xmm12 , [rcx].nvcontext._xmm12  ; ^
  movdqa  xmm13 , [rcx].nvcontext._xmm13  ; ^
  movdqa  xmm14 , [rcx].nvcontext._xmm14  ; ^
  movdqa  xmm15 , [rcx].nvcontext._xmm15  ; ^

  mov  rbx , [rcx].nvcontext._rbx  ; restore integer registers
  mov  rbp , [rcx].nvcontext._rbp  ; ^
  mov  rsi , [rcx].nvcontext._rsi  ; ^
  mov  rdi , [rcx].nvcontext._rdi  ; ^
  mov  r12 , [rcx].nvcontext._r12  ; ^
  mov  r13 , [rcx].nvcontext._r13  ; ^
  mov  r14 , [rcx].nvcontext._r14  ; ^
  mov  r15 , [rcx].nvcontext._r15  ; ^

  mov  rsp , [rcx].nvcontext._rsp  ; run the context
  mov  rcx , [rcx].nvcontext._rip  ; ^
  jmp  rcx                         ; ^  

cpprtl_eh_start_context endp

END

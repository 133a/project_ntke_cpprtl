;============================================
; copyright (c) 2012-2022 project_ntke_cpprtl
; license: the MIT license
;--------------------------------------------


;==========================;
; __CxxFrameHandler stuff  ;
;   ddk3790 support        ;
;--------------------------;

.code

extern __CxxFrameHandler3 : proc

__CxxFrameHandler proc public
  jmp __CxxFrameHandler3
__CxxFrameHandler endp

END

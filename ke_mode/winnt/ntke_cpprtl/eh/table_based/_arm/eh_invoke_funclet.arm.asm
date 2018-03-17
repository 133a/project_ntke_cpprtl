;/////////////////////////////////////////////////////////////////////////////
;////    copyright (c) 2012-2017 project_ntke_cpprtl
;////    mailto:kt133a@seznam.cz
;////    license: the MIT license
;/////////////////////////////////////////////////////////////////////////////


#include <ksarm.h>


    ;  r0  void* (*)(void)     -  the funclet entry point;
    ;  r1  void*               -  the function frame pointer the funclet is linked with;
    ;  r2  void* []            -  the {r4-r11} nv-context to be set before the funclet is invoked;
    NESTED_ENTRY _CPPRTL_invoke_funclet

        PROLOG_PUSH   {r1, r4-r11, lr}   ; keep the frame pointer, the nv-context and the link address.

        ldm    r2 , {r4-r11}             ; load the funclet {r4-r11} nv-context,
        blx    r0                        ; invoke the funclet.

        EPILOG_POP    {r3-r11, pc}       ; load 'pc' with the return address, restore the nv-context and pop out the frame ptr,
                                         ; r0 - the continuation address if any.
    NESTED_END _CPPRTL_invoke_funclet


    END


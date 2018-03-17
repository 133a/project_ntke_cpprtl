;/////////////////////////////////////////////////////////////////////////////
;////    copyright (c) 2012-2017 project_ntke_cpprtl
;////    mailto:kt133a@seznam.cz
;////    license: the MIT license
;/////////////////////////////////////////////////////////////////////////////


#include <ksarm.h>


#ifndef NT_KERNEL_MODE
#  error NT_KERNEL_MODE is required
#endif


//  'aux_::nv_context' offsets:
#define CTX_R0     0x00
#define CTX_R2     0x08
#define CTX_SP     0x34
#define CTX_LR     0x38
#define CTX_PC     0x3C
#define CTX_CPSR   0x40
#define CTX_FPSCR  0x44
#define CTX_D0     0x48
#define CTX_D16    0xC8
#define CTX_D_CNT  0x20
#define sizeof_2reg 8

//  an addition to <ksarm.h> constants
#define CPSRC_BIG_ENDIAN   0x100  // CPSR:bit8 1 - big endian, 0 - little endian (woa required) (CPSRC_ABORT in <ksarm.h>)
#define CPSRC_FIQ_DISABLE  0x40   // CPSR:bit6


    ;  r0  aux_::nv_context*  -  the context to be restored, the 'ctx.Pc' is expected to point at the continuation address
    NESTED_ENTRY _CPPRTL_restore_context

        PROLOG_PUSH {r4-r6, lr}

Check_FP_state
        CP_READ r1 , CP15_CPACR                    ;  read the 'CPACR'.
        tst     r1 , #CP15_CPACR_VFP_MASK          ;  check the FP is enabled.
        bne     Restore_D                          ;
        CP_READ r2 , CP15_TPIDRURO                 ;  read the thread local.
        bic     r2 , #CP15_THREAD_RESERVED_MASK    ;  check the TEB-ptr is here.
        cbz     r2 , Restore_R                     ;

Copy_CTX_D                                         ;  copy the 'D0-D31' from the supplied context into the 'KARM_VFP_STATE.VfpD[]'.
        ldr     r2 , [r2 , #ThVfpState]            ;  load the current 'KARM_VFP_STATE*' from the TEB.
        cbz     r2 , Restore_R                     ;
        ldr     r3 , [r0 , #CTX_FPSCR]             ;  load the 'FPSCR' from the supplied context record,
        str     r3 , [r2 , #VsFpscr]               ;  save the 'FPSCR' into the current 'KARM_VFP_STATE',
        mov     r3 , #CTX_D_CNT                    ;  the 'D' counter,
        add     r1 , r0 , #CTX_D0                  ;  'r1' - the source pointer,
        add     r2 , r2 , #VsVfpD                  ;  'r2' - the target pointer,

Copy_CTX_D_loop                                    ;  
        ldrd    r5 , r6 , [r1] , #sizeof_2reg      ;  load with the counter post-addressing ...
        strd    r5 , r6 , [r2] , #sizeof_2reg      ;  ^ store with the counter post-addressing ...
        subs    r3 , r3 , #1                       ;  ^ and ...
        bne     Copy_CTX_D_loop                    ;  ^ repeat ^
        b       Restore_R                          ;  go to the 'R'-registers restore.

Restore_D
        add     r1  , r0 , #CTX_D0                 ;  load 'D's from the supplied context
        vldm    r1! , {d0-d15}                     ;  ^
        vldm    r1  , {d16-d31}                    ;  ^
        ldr     r1  , [r0 , #CTX_FPSCR]            ;  load the 'FPSCR' from the supplied context,
        bic     r1  , r1 , #FPSCRM_DEPRECATED      ;  clear the things,
        vmsr    fpscr , r1                         ;  fill the register with the value.

Restore_R
        cpsid   i                                  ;  turn the 'IRQ'-interrupts off.
        ldr     lr , [r0 , #CTX_LR]                ;  cut the link- and
        ldr     sp , [r0 , #CTX_SP]                ;  stack facilities of this scope off.
        cps     #CPSRM_INT                         ;  set the mode.
        ldr     r1 , [r0 , #CTX_PC]                ;  load the continuation address
        bic     r1 , r1 , #1                       ;  and align it.
        ldr     r2 , [r0 , #CTX_CPSR]                               ;  load the 'CPSR'
        bic     r2 , r2 , #CPSRM_MASK                               ;  and adjust it
        bic     r2 , r2 , #(CPSRC_FIQ_DISABLE:OR:CPSRC_BIG_ENDIAN)  ;  ^ allow FIQ and set little-endian
        orr     r2 , r2 , #(CPSRM_SVC:OR:CPSRC_THUMB)               ;  ^ running thumb2 at svc mode (kernel).
        sub     sp , sp , #MachineFrameLength      ;  shape the 'rfe'-frame layout 
        strd    r1 , r2 , [sp]                     ;  ^
        add     r1 , r0 , #CTX_R2                  ;  restore {r0-r12}
        ldm     r1 , {r2-r12}                      ;  ^
        ldrd    r0 , r1 , [r0]                     ;  ^
        rfe     sp!                                ;  'return from exception'.

        EMIT_BREAKPOINT                            ;  no way out ?

        EPILOG_POP {r4-r6, pc}                     ;  that is a fake 'EPILOG_' record because of this function finally does step into the context supplied

    NESTED_END _CPPRTL_restore_context


    END


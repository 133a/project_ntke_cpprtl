;/////////////////////////////////////////////////////////////////////////////
;////    copyright (c) 2012-2016 project_ntke_cpprtl
;////    mailto:kt133a@seznam.cz
;////    license: the MIT license
;/////////////////////////////////////////////////////////////////////////////


#include <ksarm.h>


;
;  EFH_frame struct
;      DspCtx     dword  ?
;      Unwind     dword  ?
;      __fill__   dword  ?
;  EFH_frame endS
;
#define  EFH_DspCtx  0x0
#define  EFH_Unwind  0x4
#define  sizeof_EHF  0xC
#define  offset_EHF  0x10


    ;  r0  EXCEPTION_RECORD*    -  a new exception record
    ;  r1  void*                -  the executor's frame this handler is being invoked for
    ;  r2  CONTEXT*             -  the context record, not used at this function, instead the 'r2' is used as a volatile register for temporary needs
    ;  r3  DISPATCHER_CONTEXT*  -  the dispatcher context of the new dispatching loop
    LEAF_ENTRY _CPPRTL_execute_frame_handler_handler

        sub    r1 , r1, #offset_EHF              ; adjust the 'r1' to point to the 'EFH_frame'
        ldr    r2 , [r1 , #EFH_Unwind]           ; check if the executor this handler responsible for was invoked while an unwinding phase
        cbnz   r2 , EHunwind                     ; go to perform the preemption of the dispatcher context,
        ldr    r2 , [r0 , #ErExceptionFlags]     ; else check if the current exception record is 
        tst    r2 , #EXCEPTION_UNWIND            ; involved in the unwinding,
        bne    EHretContinueSearch               ; if so when we are just to return the 'ContinueSearch'.

EHnested                                            ; here to process the nested exception
        ldr    r2 , [r1 , #EFH_DspCtx]              ; get the saved dispatcher context from the previous executor frame,
        ldr    r2 , [r2 , #DcEstablisherFrame]      ; load the frame the previous dispatching loop has been excepted on
        str    r2 , [r3 , #DcEstablisherFrame]      ; and tell it to the current dispatching loop,
        ldr    r0 , =ExceptionNestedException       ; so the disposition is the 'NestedException'.
        b      EHret                                ; proceed to the return.

EHunwind
        ldr    r0 , [r1 , #EFH_DspCtx]                ; copy the previous loop dispatcher context into the current, excepting TargetPc
        ldr    r2 , [r0 , #DcControlPc]               ; ^
        str    r2 , [r3 , #DcControlPc]               ; ^
        ldr    r2 , [r0 , #DcImageBase]               ; ^
        str    r2 , [r3 , #DcImageBase]               ; ^
        ldr    r2 , [r0 , #DcFunctionEntry]           ; ^
        str    r2 , [r3 , #DcFunctionEntry]           ; ^
        ldr    r2 , [r0 , #DcEstablisherFrame]        ; ^
        str    r2 , [r3 , #DcEstablisherFrame]        ; ^
        ldr    r2 , [r0 , #DcContextRecord]           ; ^
        str    r2 , [r3 , #DcContextRecord]           ; ^
        ldr    r2 , [r0 , #DcLanguageHandler]         ; ^
        str    r2 , [r3 , #DcLanguageHandler]         ; ^
        ldr    r2 , [r0 , #DcHandlerData]             ; ^
        str    r2 , [r3 , #DcHandlerData]             ; ^
        ldr    r2 , [r0 , #DcHistoryTable]            ; ^
        str    r2 , [r3 , #DcHistoryTable]            ; ^
        ldr    r2 , [r0 , #DcScopeIndex]              ; ^
        str    r2 , [r3 , #DcScopeIndex]              ; ^
        ldrb   r2 , [r0 , #DcControlPcIsUnwound]      ; ^  <-- ControlPcIsUnwound is of a 'BOOLEAN' type, equal to 'char'
        strb   r2 , [r3 , #DcControlPcIsUnwound]      ; ^  ^
        ldr    r2 , [r0 , #DcNonVolatileRegisters]    ; ^
        str    r2 , [r3 , #DcNonVolatileRegisters]    ; ^
        ldr    r0 , =ExceptionCollidedUnwind          ; the disposition is the 'CollidedUnwind'.
        b      EHret                                  ; proceed to the return.

EHretContinueSearch
        ldr    r0 , =ExceptionContinueSearch    ; load the return value and step to the return

EHret
        EPILOG_RETURN           ; return the disposition at 'r0'.

    LEAF_END _CPPRTL_execute_frame_handler_handler




    ;  r0  EXCEPTION_RECORD*    -  the current exception record
    ;  r1  void*                -  the frame the language-specific handler to be invoked for
    ;  r2  CONTEXT*             -  the context record (of the throwed function on dispatching, and of the function to be unwound while unwinding phase)
    ;  r3  DISPATCHER_CONTEXT*  -  the current dispatching loop context
    NESTED_ENTRY _CPPRTL_execute_frame_handler , , _CPPRTL_execute_frame_handler_handler

        PROLOG_PUSH {lr}                ; save regs,
        PROLOG_STACK_ALLOC sizeof_EHF   ; alloc stack.

        str    r3 , [sp , #EFH_DspCtx]        ; put the current dispatcher context on the stack to be accessed by the exception handler of this scope,
        mov    lr , #0                        ; clear the
        str    lr , [sp , #EFH_Unwind]        ; unwind flag,
        ldr    lr , [r0 , #ErExceptionFlags]  ; detect the executor is being
        tst    lr , #EXCEPTION_UNWIND         ; invoked at the dispatching or unwinding phase
        beq    EHexecute                      ; and
        mov    lr , #1                        ; save the flag on 
        str    lr , [sp , #EFH_Unwind]        ; the current stack frame.

EHexecute                                           ; all the params for handler invoking are already in registers, so just
        ldr    lr , [r3 , #DcLanguageHandler]       ; load the frame handler address,
        blx    lr                                   ; invoke the language-specific handler supplied by the dispatching loop, 'r0' - the result to be returned.
        mov    r0 , r0                              ; 'nop' - TODO RtlVirtualUnwind() requires this ?

        EPILOG_STACK_FREE sizeof_EHF   ; remove the EHF_frame,
        EPILOG_POP {pc}                ; restore regs, return the frame disposition at the 'r0'.

    NESTED_END _CPPRTL_execute_frame_handler


    END


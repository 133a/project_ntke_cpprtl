/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_KM_DDK_H_
#define EH_KM_DDK_H_


#if DDK_VER < 6000
  extern "C"
  {
    #include <ntddk.h>
  }
#else
    #include <ntddk.h>
#endif



#ifndef    EXCEPTION_UNWINDING
#define    EXCEPTION_UNWINDING        0x2
#endif

#ifndef    EXCEPTION_NESTED_CALL
#define    EXCEPTION_NESTED_CALL      0x10
#endif

#ifndef    EXCEPTION_TARGET_UNWIND
#define    EXCEPTION_TARGET_UNWIND    0x20
#endif

#ifndef    EXCEPTION_COLLIDED_UNWIND
#define    EXCEPTION_COLLIDED_UNWIND  0x40
#endif

#ifndef    EXCEPTION_UNWIND
#define    EXCEPTION_UNWIND           0x66
#endif

#ifndef    STATUS_UNWIND_CONSOLIDATE
#define    STATUS_UNWIND_CONSOLIDATE 0x80000029L
#endif



#ifndef  UNW_FLAG_NHANDLER
#define  UNW_FLAG_NHANDLER 0x0
#endif

#ifndef  UNW_FLAG_EHANDLER
#define  UNW_FLAG_EHANDLER 0x1
#endif

#ifndef  UNW_FLAG_UHANDLER
#define  UNW_FLAG_UHANDLER 0x2
#endif
    


#ifndef    UNWIND_HISTORY_TABLE_SIZE
#define    UNWIND_HISTORY_TABLE_SIZE 12
#endif


typedef struct _RUNTIME_FUNCTION
{
  DWORD32  BeginAddress;
  DWORD32  EndAddress;
  DWORD32  UnwindData;
}
RUNTIME_FUNCTION;


typedef struct _UNWIND_HISTORY_TABLE_ENTRY
{
  DWORD64              ImageBase;
  RUNTIME_FUNCTION  *  FunctionEntry;
}
UNWIND_HISTORY_TABLE_ENTRY; 


typedef struct _UNWIND_HISTORY_TABLE
{
  DWORD32  Count;
  CHAR     LocalHint;
  CHAR     GlobalHint;
  CHAR     Search;
  CHAR     Once;
  DWORD64  LowAddress;
  DWORD64  HighAddress;
  UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
}
UNWIND_HISTORY_TABLE;


typedef struct _DISPATCHER_CONTEXT
{
  DWORD64                 ControlPc;
  DWORD64                 ImageBase;
  RUNTIME_FUNCTION     *  FunctionEntry;
  DWORD64                 EstablisherFrame;
  DWORD64                 TargetIp;
  CONTEXT              *  ContextRecord;
  PEXCEPTION_ROUTINE      LanguageHandler;
  PVOID                   HandlerData;
  UNWIND_HISTORY_TABLE *  HistoryTable;
  DWORD32                 ScopeIndex;
  DWORD32                 Fill0;
}
DISPATCHER_CONTEXT;



extern "C"
{

  RUNTIME_FUNCTION*  RtlLookupFunctionEntry
  (
    DWORD64                  ControlPc
  , DWORD64               *  ImageBase
  , UNWIND_HISTORY_TABLE  *  HistoryTable
  );


  VOID  RtlUnwindEx
  (
    PVOID                    TargetFrame
  , PVOID                    TargetIp
  , EXCEPTION_RECORD      *  ExceptionRecord
  , PVOID                    ReturnValue
  , CONTEXT               *  ContextRecord
  , UNWIND_HISTORY_TABLE  *  HistoryTable
  );


  PVOID  RtlPcToFileHeader
  (
    PVOID     PcValue
  , PVOID  *  BaseOfImage
  );
 

  VOID  RtlRaiseException
  (
    EXCEPTION_RECORD* ExceptionRecord
  );


  VOID  RtlCaptureContext
  (
    CONTEXT  *  ContextRecord
  );


  PEXCEPTION_ROUTINE  RtlVirtualUnwind
  (
    DWORD32               HandlerType
  , DWORD64               ImageBase
  , DWORD64               ControlPc
  , RUNTIME_FUNCTION   *  FunctionEntry
  , CONTEXT            *  ContextRecord
  , PVOID              *  HandlerData
  , DWORD64            *  EstablisherFrame
  , struct KNONVOLATILE_CONTEXT_POINTERS  *  ContextPointers
  );


  VOID  RtlRestoreContext
  (
    CONTEXT           *  ContextRecord,
    EXCEPTION_RECORD  *  ExceptionRecord
  );


}  // extern "C"



#endif  // include guard


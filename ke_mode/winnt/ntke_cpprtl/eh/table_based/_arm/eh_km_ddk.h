/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_KM_DDK_H_
#define EH_KM_DDK_H_


#if !defined (DDK_VER) || (DDK_VER < 9000)
#  error unsupported toolset
#else
#  include <ntddk.h>
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
  DWORD32              ImageBase;
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
  DWORD32  LowAddress;
  DWORD32  HighAddress;
  UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
}
UNWIND_HISTORY_TABLE;


typedef struct _DISPATCHER_CONTEXT
{
  DWORD32                 ControlPc;
  DWORD32                 ImageBase;
  RUNTIME_FUNCTION     *  FunctionEntry;
  DWORD32                 EstablisherFrame;
  DWORD32                 TargetPc;
  CONTEXT              *  ContextRecord;
  PEXCEPTION_ROUTINE      LanguageHandler;
  PVOID                   HandlerData;
  UNWIND_HISTORY_TABLE *  HistoryTable;
  DWORD32                 ScopeIndex;
  BOOLEAN                 ControlPcIsUnwound;
  UCHAR                *  NonVolatileRegisters;
  DWORD32                 Reserved;
}
DISPATCHER_CONTEXT;



extern "C"
{

  RUNTIME_FUNCTION*  RtlLookupFunctionEntry
  (
    ULONG_PTR                ControlPc
  , DWORD32               *  ImageBase
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
  , DWORD32               ImageBase
  , DWORD32               ControlPc
  , RUNTIME_FUNCTION   *  FunctionEntry
  , CONTEXT            *  ContextRecord
  , PVOID              *  HandlerData
  , ULONG              *  EstablisherFrame
  , struct KNONVOLATILE_CONTEXT_POINTERS  *  ContextPointers
  );


  VOID  RtlRestoreContext
  (
    CONTEXT           *  ContextRecord,
    EXCEPTION_RECORD  *  ExceptionRecord
  );


}  //  extern "C"



#endif // include guard


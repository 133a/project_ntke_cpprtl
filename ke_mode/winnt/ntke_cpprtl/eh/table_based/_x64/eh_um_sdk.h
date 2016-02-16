/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_UM_SDK_H_
#define EH_UM_SDK_H_


#include <windows.h>


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



typedef 
  EXCEPTION_DISPOSITION (* PEXCEPTION_ROUTINE)
  (
    EXCEPTION_RECORD*
  , PVOID
  , CONTEXT*
  , PVOID
  );


////  some stuff is defined currently only in WinSDK >=8.0 (um header winnt.h) which is shipped with the msvc2012 and higher (mscl version >=17)

#if ( defined ( _MSC_VER ) && _MSC_VER < 1700 )  ||  !defined (_MSC_VER)
#  if !defined ( __CODEGEARC__ )
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


extern "C" __declspec(dllimport)
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
#  endif  // __CODEGEARC__

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

#endif  // _MSC_VER



extern "C"
{

  __declspec(dllimport)
  RUNTIME_FUNCTION*  RtlLookupFunctionEntry
  (
    DWORD64                  ControlPc
  , DWORD64               *  ImageBase
  , UNWIND_HISTORY_TABLE  *  HistoryTable
  );


  __declspec(dllimport)
  VOID  RtlUnwindEx
  (
    PVOID                    TargetFrame
  , PVOID                    TargetIp
  , EXCEPTION_RECORD      *  ExceptionRecord
  , PVOID                    ReturnValue
  , CONTEXT               *  ContextRecord
  , UNWIND_HISTORY_TABLE  *  HistoryTable
  );


  __declspec(dllimport)
  PVOID  RtlPcToFileHeader
  (
    PVOID     PcValue
  , PVOID  *  BaseOfImage
  );


}  //  extern "C"


#endif // include guard


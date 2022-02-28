//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_FRAMEWORK_UM_SDK_H_
#define EH_FRAMEWORK_UM_SDK_H_


#include <windows.h>


#define EXCEPTION_UNWINDING     0x2
#define EXCEPTION_TARGET_UNWIND 0x20
#ifndef EXCEPTION_UNWIND
#define EXCEPTION_UNWIND        0x66
#endif

#define UNW_FLAG_NHANDLER 0x0
#define UNW_FLAG_EHANDLER 0x1
#define UNW_FLAG_UHANDLER 0x2

#define UNWIND_HISTORY_TABLE_SIZE 12

typedef 
  EXCEPTION_DISPOSITION (*PEXCEPTION_ROUTINE)
  (
    EXCEPTION_RECORD*
  , PVOID
  , CONTEXT*
  , PVOID
  );

//  some stuff isn't defined in WinSDK<7.1 / WDK<6001 (um header winnt.h)
#if ( defined (DDK_VER) && (DDK_VER < 6000) ) || ( !defined(DDK_VER) && !(_MSC_VER >= 1600) )
typedef struct _UNWIND_HISTORY_TABLE_ENTRY
{
  ULONG_PTR         ImageBase;
  RUNTIME_FUNCTION* FunctionEntry;
}
UNWIND_HISTORY_TABLE_ENTRY; 

typedef struct _UNWIND_HISTORY_TABLE
{
  ULONG     Count;
  UCHAR     LocalHint;
  UCHAR     GlobalHint;
  UCHAR     Search;
  UCHAR     Once;
  ULONG_PTR LowAddress;
  ULONG_PTR HighAddress;
  UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
}
UNWIND_HISTORY_TABLE;

extern "C" __declspec(dllimport)
PEXCEPTION_ROUTINE RtlVirtualUnwind
(
  ULONG             HandlerType
, ULONG_PTR         ImageBase
, ULONG_PTR         ControlPc
, RUNTIME_FUNCTION* FunctionEntry
, CONTEXT*          ContextRecord
, PVOID*            HandlerData
, ULONG_PTR*        EstablisherFrame
, struct KNONVOLATILE_CONTEXT_POINTERS* ContextPointers
);
#endif

#if !(_MSC_VER >= 1700)
typedef struct _DISPATCHER_CONTEXT
{
  ULONG_PTR             ControlPc;
  ULONG_PTR             ImageBase;
  RUNTIME_FUNCTION*     FunctionEntry;
  ULONG_PTR             EstablisherFrame;
  ULONG_PTR             TargetIp;
  CONTEXT*              ContextRecord;
  PEXCEPTION_ROUTINE    LanguageHandler;
  PVOID                 HandlerData;
  UNWIND_HISTORY_TABLE* HistoryTable;
  ULONG                 ScopeIndex;
  ULONG                 Fill0;
}
DISPATCHER_CONTEXT;
#endif

extern "C"
{
  __declspec(dllimport)
  RUNTIME_FUNCTION* RtlLookupFunctionEntry
  (
    ULONG_PTR             ControlPc
  , ULONG_PTR*            ImageBase
  , UNWIND_HISTORY_TABLE* HistoryTable
  );

  __declspec(dllimport)
  PVOID RtlPcToFileHeader
  (
    PVOID  PcValue
  , PVOID* BaseOfImage
  );
}  // extern "C"


#endif  // include guard

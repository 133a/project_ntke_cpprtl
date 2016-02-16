/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_UM_SDK_H_
#define EH_UM_SDK_H_


#include <windows.h>

#if 00000000  //  all the things are now available at the WinSDK um-headers

#ifndef    EXCEPTION_UNWINDING
#define    EXCEPTION_UNWINDING      0x2
#endif

#ifndef    EXCEPTION_TARGET_UNWIND
#define    EXCEPTION_TARGET_UNWIND  0x20
#endif

#ifndef    EXCEPTION_UNWIND
#define    EXCEPTION_UNWIND         0x66
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



extern "C"
{

  __declspec(dllimport)
  RUNTIME_FUNCTION*  RtlLookupFunctionEntry
  (
    ULONG_PTR                ControlPc
  , DWORD                 *  ImageBase
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

#endif  //  00000000

#endif // include guard


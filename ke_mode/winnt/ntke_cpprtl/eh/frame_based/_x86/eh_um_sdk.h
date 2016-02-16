/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_UM_SDK_H_
#define EH_UM_SDK_H_


#include <windows.h>


#ifndef   EXCEPTION_UNWIND
#define   EXCEPTION_UNWIND     0x66
#endif

#ifndef   EXCEPTION_UNWINDING
#define   EXCEPTION_UNWINDING  0x2
#endif

#ifndef   EXCEPTION_NESTED_CALL
#define   EXCEPTION_NESTED_CALL 0x10
#endif


typedef 
  EXCEPTION_DISPOSITION (__stdcall * PEXCEPTION_ROUTINE)
  (
    EXCEPTION_RECORD*
  , PVOID
  , CONTEXT*
  , PVOID
  );

   
extern "C"
{

  __declspec(dllimport) 
  VOID __stdcall  RtlUnwind
  (
    PVOID                TargetFrame
  , PVOID                TargetIp
  , EXCEPTION_RECORD  *  ExceptionRecord
  , PVOID                ReturnValue
  );

}


#endif // include guard


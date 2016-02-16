/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
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


#ifndef   EXCEPTION_UNWINDING
#define   EXCEPTION_UNWINDING  0x2
#endif

#ifndef   EXCEPTION_UNWIND
#define   EXCEPTION_UNWIND     0x66
#endif

#ifndef   EXCEPTION_NESTED_CALL
#define   EXCEPTION_NESTED_CALL 0x10
#endif

    
extern "C"
{

  VOID __stdcall  RtlRaiseException
  (
    EXCEPTION_RECORD* ExceptionRecord
  );


  VOID __stdcall  RtlUnwind
  (
    PVOID                TargetFrame
  , PVOID                TargetIp
  , EXCEPTION_RECORD  *  ExceptionRecord
  , PVOID                ReturnValue
  );

}  //  extern "C"


#endif // include guard


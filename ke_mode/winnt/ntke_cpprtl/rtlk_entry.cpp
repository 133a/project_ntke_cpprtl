//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtl_framework.h"
#include "rtl_init.h"


//=================
// RTL unload point
//-----------------
namespace 
{
  PDRIVER_UNLOAD clientDriverUnload = 0;

  // kernel module unload point
  void DriverUnload(DRIVER_OBJECT* pDriverObject)
  {
    if ( clientDriverUnload )
    {
      clientDriverUnload(pDriverObject);  // invoke the rtl client's unload routine
    }
    cpprtl::stop();
  }
}


//================
// RTL entry point
//----------------
extern "C"
{
  // kernel module entry point to be defined by a client of ntke_cpprtl
  NTSTATUS ntkeDriverEntry(DRIVER_OBJECT*, UNICODE_STRING*);

  // kernel module entry point
  extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* pDriverObject, UNICODE_STRING* pRegistryPath)
  {
    NTSTATUS status = cpprtl::start();
    if ( NT_SUCCESS(status) )
    {
      status = ntkeDriverEntry(pDriverObject, pRegistryPath);  // invoke the rtl client's entry point
      if ( !NT_SUCCESS(status) )
      {
        goto RTL_exit_on_error;  // shutdown rtl right here since the unload routine will not be invoked
      }
      clientDriverUnload = pDriverObject->DriverUnload;  // save the client's unload routine
      pDriverObject->DriverUnload = &DriverUnload;       // override the client's unload routine
      return status;
    }
  RTL_exit_on_error:
    cpprtl::stop();
    return status;
  }
}  // extern "C"


extern "C"
{
  int _charmax = 255;

#if ( _MSC_VER >= 1900 )  // ucrt referred stuff
  int __do_unsigned_char_lconv_initialization = 0;
#endif
}  // extern "C"

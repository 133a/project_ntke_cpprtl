/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtl_framework_specific_header.h"
#include "rtl_init.h"


/////////////////////////////////////////////////////////////////////////////
////  RTL unload point
/////////////////////////////////////////////////////////////////////////////

namespace 
{
  PDRIVER_UNLOAD DriverUnload_client = 0;

  // kernel module unload point
  void _NTKE_CPPRTL_DriverUnload(DRIVER_OBJECT* pDriverObject)
  {
    if ( DriverUnload_client )
    {
      DriverUnload_client(pDriverObject);  // invoke the rtl client's unload routine
    }
    cpprtl::stop();
  }
}


/////////////////////////////////////////////////////////////////////////////
////  RTL entry point
/////////////////////////////////////////////////////////////////////////////

// kernel module entry point to be defined by a client of ntke_cpprtl
extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT*, UNICODE_STRING*);


// kernel module entry point
extern "C" NTSTATUS _NTKE_CPPRTL_DriverEntry(DRIVER_OBJECT* pDriverObject, UNICODE_STRING* pRegistryPath)
{
  NTSTATUS status = cpprtl::start();
  if ( NT_SUCCESS(status) )
  {
    status = DriverEntry(pDriverObject, pRegistryPath);  // invoke the rtl client's entry point
    if ( ! NT_SUCCESS(status) )
    {
      goto RTL_exit_on_error;  // shutdown rtl right here since the unload routine will not be invoked
    }
    DriverUnload_client = pDriverObject->DriverUnload;  // save the client's unload routine
    pDriverObject->DriverUnload = &_NTKE_CPPRTL_DriverUnload;  // override the client's unload routine
    return status;
  }

RTL_exit_on_error:
  cpprtl::stop();
  return status;
}


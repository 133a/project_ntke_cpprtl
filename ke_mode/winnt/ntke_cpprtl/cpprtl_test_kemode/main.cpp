/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include "run_tests.h"


extern ULONG const ntke_memtag = 'TRPC';  //  'CPRT'


/////////////////////////////////////////////////////////////////////////////
//// entry/unload points
/////////////////////////////////////////////////////////////////////////////

static void DriverUnload(DRIVER_OBJECT* pDriverObject)
{
  DbgPrint("cpprtl_test.DriverUnload()");
}


extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* pDriverObject, UNICODE_STRING* /*pRegistryPath*/)
{
  DbgPrint("cpprtl_test.DriverEntry()\n");
  pDriverObject->DriverUnload = &DriverUnload;
  int ret = cpprtl_tests::run();
  DbgPrint("cpprtl_test.DriverEntry() : cpprtl_tests::run()=%i\n", ret);
  return 0 == ret ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}


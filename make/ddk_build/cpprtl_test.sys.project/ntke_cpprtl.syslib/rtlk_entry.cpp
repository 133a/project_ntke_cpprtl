#define  DriverEntry  _NTKE_CPPRTL_user_defined_driver_entry
#  include <../ntke_cpprtl/rtlk_entry.cpp>
#undef   DriverEntry

extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* pDriverObject, UNICODE_STRING* pRegistryPath)
{
  return _NTKE_CPPRTL_DriverEntry(pDriverObject, pRegistryPath);
}

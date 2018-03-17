/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


namespace cpprtl
{
namespace eh
{
namespace aux_
{

////   memcpy
  void memcpy(void* const d, void const* const s, ::size_t sz)
  {
  #ifdef NT_KERNEL_MODE
    RtlMoveMemory(d, s, sz);  // any IRQL for the np-memory
  #else
    if (char const* const src = reinterpret_cast<char const*>(s))
    {
      if (char* const dst = reinterpret_cast<char*>(d))
      {
        while (sz--)
        {
          dst[sz] = src[sz];
        }
      }
    }
  #endif
  }


////   compare zero-terminated ansi-strings
////   TODO: use kernel Rtl???()
  bool strzcmp(char const* ch1, char const* ch2)
  {
    while ( *ch1 || *ch2 )
    {
      if ( *(ch1++) != *(ch2++) )
      {
        return false;
      }
    }
    return true;
  }


////   make the stack traversing started
  void raise_exception(::EXCEPTION_RECORD& exc_rec)
  {
  #ifdef NT_KERNEL_MODE
  //  KIRQL irql = KeGetCurrentIrql();
  //  ::size_t r_stack = IoGetRemainingStackSize();
    IRQL_CHECK ( <=DISPATCH_LEVEL )  // RtlRaiseException()
    RtlRaiseException(&exc_rec);
  #else
    ::RaiseException(exc_rec.ExceptionCode, exc_rec.ExceptionFlags, exc_rec.NumberParameters, exc_rec.ExceptionInformation);
  #endif
  }


////   terminate the process execution if the exception handling goes a wrong way
  void invalid_exception()
  {
  #ifdef NT_KERNEL_MODE
    KeBugCheckEx(eh::EXCEPTION_CODE_CPP, 0, 0, 0, 0);
  #else
    ::ExitProcess(eh::EXCEPTION_CODE_CPP);
  #endif
  }


  void invalid_exception(int const& exc_code, int const& exc_subcode)
  {
  #ifdef NT_KERNEL_MODE
    KeBugCheckEx(exc_code, exc_subcode, 0, 0, 0);
  #else
    ::ExitProcess(exc_code);
  #endif
  }

}  // namespace aux_
}  // namespace eh
}  // namespace cpprtl


extern "C"
{
#if ( _MSC_VER >= 1900 )  // msvc2015/wdk10 (upd3) compiler predeclared
  extern "C" void __cdecl __std_terminate()
  {
    using namespace cpprtl;
    eh::aux_::invalid_exception(eh::EXCEPTION_CODE_CPP, eh::EXCEPTION_SUBCODE_STD_TERMINATE_INTERNAL);
  }
#endif
}  // extern "C"


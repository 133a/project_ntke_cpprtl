//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_framework.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


namespace cpprtl { namespace eh { namespace aux_
{

  void memcpy(void* const d, void const* const s, size_t sz)
  {
  #ifdef NT_KERNEL_MODE
    RtlMoveMemory(d, s, sz);  // any IRQL for the np-memory
  #else
    typedef unsigned char uchar;
    if (uchar const* const src = reinterpret_cast<uchar const*>(s))
    {
      if (uchar* const dst = reinterpret_cast<uchar*>(d))
      {
        while (sz--)
        {
          dst[sz] = src[sz];
        }
      }
    }
  #endif
  }

  // TODO: use kernel Rtl???()
  bool strzeq(char const* ch1, char const* ch2)
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

  __declspec(noreturn)
  void terminate()
  {
  #ifdef NT_KERNEL_MODE
    KeBugCheckEx(eh::EXCEPTION_CODE_CPP, 0, 0, 0, 0);
  #else
    ::ExitProcess(eh::EXCEPTION_CODE_CPP);
  #endif
  }

  __declspec(noreturn)
  void terminate(int const exc_code, int const exc_subcode)
  {
  #ifdef NT_KERNEL_MODE
    KeBugCheckEx(exc_code, exc_subcode, 0, 0, 0);
  #else
    ::ExitProcess(exc_code);
  #endif
  }

}}}  // namespace cpprtl::eh::aux_


extern "C"
{
#if (_MSC_VER >= 1900)  // msvc2015/wdk10 (upd3) compiler predeclared, handles inlined noexcept functions
  extern "C" __declspec(noreturn) void __cdecl __std_terminate()
  {
    using namespace cpprtl::eh;
    aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_NOEXCEPT_THROW_INLINED);
  }
#endif  // _MSC_VER
}  // extern "C"


#ifdef __ICL  // icl array copying support
void __cdecl terminate()
{
  using namespace cpprtl::eh;
  aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STD_TERMINATE_INTERNAL);
}
#endif  // __ICL


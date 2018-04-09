/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtl_framework_specific_header.h"
#include "rtl_init_gstatic.h"
#include "eh/eh_exception_code.h"


/////////////////////////////////////////////////////////////////////////////
////  RTL global/static support
/////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER

  #if _MSC_VER < 1400
    #pragma section(".CRT$XCA", read, write)     // c++ global/static initializers .CRT-section pre-group
    #pragma section(".CRT$XCZ", read, write)     // c++ global/static initializers .CRT-section post-group
  #else
    #pragma section(".CRT$XCA", read)            // c++ global/static initializers .CRT-section pre-group
    #pragma section(".CRT$XCZ", read)            // c++ global/static initializers .CRT-section post-group
  #endif


  __declspec(allocate(".CRT$XCA")) cpprtl::gstatic::pf_t cpprtl_xc_begin[1]  = { 0 };
  __declspec(allocate(".CRT$XCZ")) cpprtl::gstatic::pf_t cpprtl_xc_end[1]    = { 0 };


  #if _MSC_VER < 1400
    #pragma comment(linker, "-MERGE:.CRT=.data")
  #else
    #pragma comment(linker, "-MERGE:.CRT=.rdata")
  #endif

#endif _MSC_VER


namespace
{

  class pf_storage  // atexit-ed funclets storage
  {
  public:
    enum { PF_STOR_SZ = 1024 };  // TODO make it customizable by the rtl client

  private:
    cpprtl::gstatic::pf_t pf_arr[PF_STOR_SZ];
    LONG idx;

  public:
    void init()
    {
      idx = 0;
    }

    // make sure the 'pf_arr' and 'idx' are thread-safely accessed 'cos put() can be invoked from the different threads by atexit()
    cpprtl::gstatic::pf_t put(cpprtl::gstatic::pf_t pf)
    {
      LONG const i = InterlockedExchangeAdd(&idx, 1);
      if ( PF_STOR_SZ > i )
      {
        return pf_arr[i] = pf;
      }
      return 0;
    }

    // thread-unsafe 'cos it is called at the driver unload point assuming all other driver's threads have been acquired
    void unwind()
    {
      NTSTATUS status = STATUS_SUCCESS;
      for ( --idx ; idx >= 0 ; --idx )
      {
        if ( pf_arr[idx] )
        {
          __try
          {
            pf_arr[idx]();
          }
          __except ( status = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER )
          {
            KeBugCheckEx(cpprtl::eh::EXCEPTION_CODE_CPP, status, idx, reinterpret_cast<ULONG_PTR>(pf_arr[idx]), 0);
          }
        }
      }
      ASSERT ( 0 > idx );
    }

  }
  pf_stor;


  // returns a status for not to end up with a bugcheck while driver loading if possible
  NTSTATUS invoke_pf(cpprtl::gstatic::pf_t* from, cpprtl::gstatic::pf_t* const to)
  {
    NTSTATUS status = STATUS_SUCCESS;
    for ( ; from < to ; ++from )
    {
      if ( *from )
      {
        __try
        {
          (*from)();
        }
        __except ( status = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER )
        {
          return NT_SUCCESS(status) ? STATUS_UNSUCCESSFUL : status;
        }
      }
    }
    return status;
  }

}  // namespace


namespace cpprtl
{
namespace gstatic
{

  int start(void)
  {
    pf_stor.init();
    ASSERT ( cpprtl_xc_end - cpprtl_xc_begin < pf_storage::PF_STOR_SZ );  // assume each start-up funclet schedules a corresponding shutdown action (e.g. dtor)
    return invoke_pf(cpprtl_xc_begin, cpprtl_xc_end);                     // invokes funclets stored in .CRT$XCU
  }


  void stop(void)
  {
    pf_stor.unwind();  // invokes funclets from pf_stor[]
  }

}  // namespace gstatic
}  // namespace cpprtl


extern "C" int __cdecl atexit(cpprtl::gstatic::pf_t pf)  // mscl internally pre-declared
{
  return 0 != pf_stor.put(pf);
}


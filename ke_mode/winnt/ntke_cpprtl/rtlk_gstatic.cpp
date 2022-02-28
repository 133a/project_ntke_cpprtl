//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtl_framework.h"
#include "rtl_init_gstatic.h"
#include "eh/eh_exception_code.h"


#ifdef _MSC_VER
  #if (_MSC_VER < 1400)
    #pragma section(".CRT$XCA", read, write)  // c++ global/static initializers .CRT-section pre-group
    #pragma section(".CRT$XCZ", read, write)  // c++ global/static initializers .CRT-section post-group
  #else
    #pragma section(".CRT$XCA", read)         // c++ global/static initializers .CRT-section pre-group
    #pragma section(".CRT$XCZ", read)         // c++ global/static initializers .CRT-section post-group
  #endif

  __declspec(allocate(".CRT$XCA")) cpprtl::gstatic::pf_t cpprtl_xc_begin[1]  = { 0 };
  __declspec(allocate(".CRT$XCZ")) cpprtl::gstatic::pf_t cpprtl_xc_end[1]    = { 0 };

  #if (_MSC_VER < 1400)
    #pragma comment(linker, "-MERGE:.CRT=.data")
  #else
    #pragma comment(linker, "-MERGE:.CRT=.rdata")
  #endif
#endif  // _MSC_VER


namespace
{
  class pf_storage  // atexit-ed funclets storage
  {
  public:
    enum { PF_STOR_LEN = 1024 };  // TODO make it customizable by the rtl client

  private:
    cpprtl::gstatic::pf_t pf_arr[PF_STOR_LEN];
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
      if ( PF_STOR_LEN > i )
      {
        return pf_arr[i] = pf;
      }
      return 0;
    }

    // thread-unsafe 'cos it is called at the driver unload point assuming all other driver's threads have been acquired
    void unwind()
    {
      while ( --idx >= 0 )
      {
        if ( pf_arr[idx] )
        {
          pf_arr[idx]();
        }
      }
      ASSERT ( 0 > idx );
    }
  }
  pf_stor;

  // returns a status for not to end up with a bugcheck while driver loading if possible
  void invoke_pf(cpprtl::gstatic::pf_t* from, cpprtl::gstatic::pf_t* const to)
  {
    for ( ; from < to ; ++from )
    {
      if ( *from )
      {
        (*from)();
      }
    }
  }
}  // namespace


namespace cpprtl { namespace gstatic
{
  bool start(void)
  {
    pf_stor.init();
    ASSERT ( cpprtl_xc_end - cpprtl_xc_begin < pf_storage::PF_STOR_LEN );  // assume each start-up funclet schedules a corresponding shutdown action (e.g. dtor)
    invoke_pf(cpprtl_xc_begin, cpprtl_xc_end);  // invokes funclets stored in .CRT$XCU
    return true;
  }

  void stop(void)
  {
    pf_stor.unwind();  // invokes funclets from pf_stor[]
  }
}}  // namespace cpprtl::gstatic


extern "C" int __cdecl atexit(cpprtl::gstatic::pf_t pf)  // mscl internally pre-declared
{
  return 0 != pf_stor.put(pf);
}

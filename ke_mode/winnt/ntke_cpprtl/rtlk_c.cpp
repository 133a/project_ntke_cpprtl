/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtl_framework_specific_header.h"
#include "rtlk_heap.h"


namespace 
{
  ULONG const rtl_memtag = 'EKTN';  // "NTKE"
}


extern "C"
{
  // malloc() and free() are used by stlport 'class __Named_exception' to maintain the buffer containing a string with an exception-related data

  void* __cdecl malloc(size_t sz)
  {
    return cpprtl::heap::alloc(sz, NonPagedPool, rtl_memtag);
  }


  void __cdecl free(void* ptr)
  {
    return cpprtl::heap::dealloc(ptr);
  }

}  // extern "C"


extern "C"
{
  int _charmax = 255;

#if ( _MSC_VER >= 1900 )  // ucrt referred stuff
  int __do_unsigned_char_lconv_initialization = 0;
#endif
}  // extern "C"


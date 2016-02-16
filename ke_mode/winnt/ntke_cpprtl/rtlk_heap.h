/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTLK_HEAP_H_
#define RTLK_HEAP_H_


#include "rtl_framework_specific_header.h"


namespace cpprtl
{
namespace heap
{

  void*     alloc(size_t const& sz, POOL_TYPE const& pt, ULONG const& tag);
  void      dealloc(void* const ptr);
  void      dealloc(void* const ptr, POOL_TYPE const& pt);

}  // namespace heap
}  // namespace cpprtl


#endif  // include guard


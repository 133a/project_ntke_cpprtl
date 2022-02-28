//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTLK_HEAP_H_
#define RTLK_HEAP_H_


#include "rtl_framework.h"


namespace cpprtl { namespace heap
{
  void* alloc(size_t const& sz, POOL_TYPE const& pt, ULONG const& tag);
  void  dealloc(void* const ptr);
  void  dealloc(void* const ptr, POOL_TYPE const& pt);
}}  // namespace cpprtl::heap


#endif  // include guard

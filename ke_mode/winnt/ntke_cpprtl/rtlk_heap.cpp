//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtl_framework.h"
#include "rtl_init_heap.h"
#include "rtlk_heap.h"


namespace
{
  bool DBG_memory_allocation_check(POOL_TYPE const pt)
  {
    bool res = false;
    switch (pt)
    {
    case NonPagedPool:
      res = KeGetCurrentIrql() <= DISPATCH_LEVEL;
      break;
    case PagedPool:
      res = KeGetCurrentIrql() <= APC_LEVEL;
      break;
    };
    return res;
  }
}  // namespace

namespace cpprtl { namespace heap
{
  bool start()
  {
    return true;
  }

  void stop()
  {
  }

  void* alloc(size_t const& sz, POOL_TYPE const& pt, ULONG const& tag)
  {
    ASSERT ( DBG_memory_allocation_check(pt) );
    return ExAllocatePoolWithTag(pt, sz, tag);
  }
  
  void dealloc(void* const ptr)
  {
    if (ptr)
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );  // but make sure IRQL <= APC_LEVEL for PagedPool
      ExFreePool(ptr);
    }
  }

  void dealloc(void* const ptr, POOL_TYPE const& pt)
  {
    if (ptr)
    {
      ASSERT ( DBG_memory_allocation_check(pt) );
      ExFreePool(ptr);
    }
  }
}}  // namespace cpprtl::heap

namespace 
{
  ULONG const rtl_memtag = 'EKTN';  // "NTKE"
}

// malloc() and free() are used by stlport 'class __Named_exception' to maintain exception-related data string
extern "C"
{
  void* __cdecl malloc(size_t sz)
  {
    return cpprtl::heap::alloc(sz, NonPagedPool, rtl_memtag);
  }

  void __cdecl free(void* ptr)
  {
    return cpprtl::heap::dealloc(ptr);
  }
}  // extern "C"

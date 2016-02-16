/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtl_framework_specific_header.h"
#include "rtl_init_heap.h"
#include "rtlk_heap.h"


/////////////////////////////////////////////////////////////////////////////
//// nt kernel heap memory allocation
/////////////////////////////////////////////////////////////////////////////

namespace
{

  bool DBG_memory_allocation_check(POOL_TYPE const& pt)
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


namespace cpprtl
{
namespace heap
{

  int start()
  {
    return STATUS_SUCCESS;
  }


  void stop()
  {
  }


  void* alloc(size_t const& sz, POOL_TYPE const& pt, ULONG const& tag)
  {
    ASSERT(DBG_memory_allocation_check(pt));
    return ExAllocatePoolWithTag(pt, sz, tag);
  }

  
  void dealloc(void* const ptr)
  {
    if (ptr)
    {
      ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);  // but make sure IRQL <= APC_LEVEL for PagedPool
      ExFreePool(ptr);
    }
  }


  void dealloc(void* const ptr, POOL_TYPE const& pt)
  {
    if (ptr)
    {
      ASSERT(DBG_memory_allocation_check(pt));
      ExFreePool(ptr);
    }
  }

}  // namespace heap
}  // namespace cpprtl


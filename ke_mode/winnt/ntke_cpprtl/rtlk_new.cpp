//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtl_framework.h"
#include "rtlk_new.h"
#include "rtlk_heap.h"


//=================
// std::new_handler
//-----------------
namespace
{
  std::new_handler volatile nh = 0;

  std::new_handler get_new_handler_impl()
  {
    return nh;
  }

  std::new_handler set_new_handler_impl(std::new_handler new_p)
  {
    return reinterpret_cast<std::new_handler>(InterlockedExchangePointer(reinterpret_cast<void* volatile*>(&nh), new_p));
  }
}

namespace std
{
  new_handler set_new_handler(new_handler new_p) throw()
  {
    return set_new_handler_impl(new_p);
  }
}


//===============
// std::nothrow_t
//---------------
// ewdk17134 and newer contain `std::nothrow' object in ntoskrnl.lib for nt10.0
#if !defined (DDK_VER)              \
  || (DDK_VER < 17134)              \
  || (NTDDI_VERSION < 0x0A000000)
namespace std
{
  extern nothrow_t const nothrow = nothrow_t();
}
#endif  // DDK_VER


//=============
// <rtlk_new.h>                                                           
//-------------
#pragma warning(push)
#pragma warning(disable:4290)  // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)

void* __cdecl operator new(std::size_t size, POOL_TYPE const& pt) throw(std::bad_alloc)
{
  for ( ;; )
  {
    void* p = cpprtl::heap::alloc(size, pt, ntke_memtag);
    if (p)
    {
      return p;
    }
    std::new_handler nh = get_new_handler_impl();
    if (0 == nh)
    {
      throw std::bad_alloc();
    }
    nh();
  }
}

void __cdecl operator delete(void* ptr, POOL_TYPE const& pt) throw()
{
  cpprtl::heap::dealloc(ptr, pt);
}


void* __cdecl operator new[](std::size_t size, POOL_TYPE const& pt) throw(std::bad_alloc)
{
  return ::operator new(size, pt);
}

void __cdecl operator delete[](void* ptr, POOL_TYPE const& pt) throw()
{
  return ::operator delete(ptr, pt);
}


void* __cdecl operator new(std::size_t size, std::nothrow_t const&, POOL_TYPE const& pt) throw()
{
  for ( ;; )
  {
    void* p = cpprtl::heap::alloc(size, pt, ntke_memtag);
    if (p)
    {
      return p;
    }
    std::new_handler nh = get_new_handler_impl();
    if (0 == nh)
    {
      return 0;
    }
    try
    {
      nh();
    }
    catch (std::bad_alloc&)
    {
      return 0;
    }
  }
}

void __cdecl operator delete(void* ptr, std::nothrow_t const&, POOL_TYPE const& pt) throw()
{
  return ::operator delete(ptr, pt);
}


void* __cdecl operator new[](std::size_t size, std::nothrow_t const& nothrow_tag, POOL_TYPE const& pt) throw()
{
  return ::operator new(size, nothrow_tag, pt);
}

void __cdecl operator delete[](void* ptr, std::nothrow_t const& nothrow_tag, POOL_TYPE const& pt) throw()
{
  return ::operator delete(ptr, nothrow_tag, pt);
}


//======
// <new>
//------
void* __cdecl operator new(std::size_t size) throw(std::bad_alloc)
{
  return ::operator new(size, NonPagedPool);
}

void __cdecl operator delete(void* ptr) throw()
{
  cpprtl::heap::dealloc(ptr);
}


void* __cdecl operator new[](std::size_t size) throw(std::bad_alloc)
{
  return ::operator new(size);
}

void __cdecl operator delete[](void* ptr) throw()
{
  return ::operator delete(ptr);
}


void* __cdecl operator new(std::size_t size, std::nothrow_t const& nothrow_tag) throw()
{
  return ::operator new(size, nothrow_tag, NonPagedPool);
}

void __cdecl operator delete(void* ptr, std::nothrow_t const&) throw()
{
  return ::operator delete(ptr);
}


void* __cdecl operator new[](std::size_t size, std::nothrow_t const& nothrow_tag) throw()
{
  return ::operator new(size, nothrow_tag);
}

void __cdecl operator delete[](void* ptr, std::nothrow_t const&) throw()
{
  return ::operator delete(ptr);
}


//===============================
// placement new/delete operators
//-------------------------------
void* __cdecl operator new(std::size_t, void* ptr) throw()
{
  return ptr;
}

void __cdecl operator delete(void*, void*) throw()
{
}


void* __cdecl operator new[](std::size_t, void* ptr) throw()
{
  return ptr;
}

void __cdecl operator delete[](void*, void*) throw()
{
}


//====================================
// sized delete operators (>=msvc2015)
//------------------------------------
#if (_MSC_VER >= 1900)
void __cdecl operator delete(void* ptr, std::size_t) throw()
{
  return ::operator delete(ptr);
}

void __cdecl operator delete[](void* ptr, std::size_t) throw()
{
  return ::operator delete(ptr);
}
#endif


#pragma warning(pop)  // disable:4290

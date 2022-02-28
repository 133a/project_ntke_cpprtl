//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTLK_NEW_H_
#define RTLK_NEW_H_


#include "rtl_framework.h"
#include "std_incl/exception"
#include "std_incl/new"


extern const ULONG ntke_memtag;


// the new() operators with their counterparts for EH memory deallocation (mscl C4291)

#pragma warning(push)
#pragma warning(disable:4290)  // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)


// scalar new/delete pooled
void* __cdecl operator new       (std::size_t size, POOL_TYPE const& pt)  throw(std::bad_alloc);
void  __cdecl operator delete    (void* ptr,        POOL_TYPE const& pt)  throw();

void* __cdecl operator new       (std::size_t size, std::nothrow_t const&, POOL_TYPE const& pt)  throw();
void  __cdecl operator delete    (void* ptr,        std::nothrow_t const&, POOL_TYPE const& pt)  throw();


// array new/delete pooled
void* __cdecl operator new[]     (std::size_t size, POOL_TYPE const& pt)  throw(std::bad_alloc);
void  __cdecl operator delete[]  (void* ptr,        POOL_TYPE const& pt)  throw();

void* __cdecl operator new[]     (std::size_t size, std::nothrow_t const&, POOL_TYPE const& pt)  throw();
void  __cdecl operator delete[]  (void* ptr,        std::nothrow_t const&, POOL_TYPE const& pt)  throw();


#pragma warning(pop)  // disable:4290


#endif  // include guard

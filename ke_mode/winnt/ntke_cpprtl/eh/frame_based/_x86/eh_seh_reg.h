//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_SEH_REG_H_
#define EH_SEH_REG_H_


#include "eh_config.h"


struct _EXCEPTION_RECORD;  // fwd

namespace cpprtl { namespace eh { namespace msvc_data { namespace seh
{

  struct registration;  // fwd
  typedef unsigned (__cdecl* handler_ft)(::_EXCEPTION_RECORD*, registration*, void*, void*);  // ::PEXCEPTION_ROUTINE look-alike

  struct registration  // generic SEH frame data
  {
    registration* next;           // linked list pointer
    handler_ft    frame_handler;  // SEH frame handler

    void verify() const;	

  private:
    void* operator new(size_t);
    void* operator new[](size_t);
  };

  __declspec(naked)
  inline registration* __fastcall head()
  {
    __asm
    {
      mov eax , fs:[0]  // NT_TIB.ExceptionList
      ret
    }
  }

  __declspec(naked)
  inline registration* __fastcall tail()
  {
    __asm
    {
      xor eax , eax
      dec eax
      ret
    }
  }

#pragma warning(push)
#pragma warning(disable:4733)  // inline asm assigning to 'fs:[0]' : handler not registered as safe handler
  __declspec(naked)
  inline void __fastcall link(registration* reg)
  {
    __asm  // ecx=reg
    {
      mov  eax , fs:[0]
      mov  [ecx].next , eax
      mov  fs:[0] , ecx
      ret
    }
  }

  __declspec(naked)
  inline registration* __fastcall unlink(registration* reg)  // returns the reg->next
  {
    __asm  // ecx=reg
    {
      mov  eax , [ecx].next
      mov  fs:[0] , eax
      ret
    }
  }
#pragma warning(pop)  // disable:4733
  
}}}}  // namespace cpprtl::eh::msvc_data::seh


#endif  // include guard

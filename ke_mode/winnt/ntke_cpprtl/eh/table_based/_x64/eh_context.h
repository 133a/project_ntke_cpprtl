//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_CONTEXT_H_
#define EH_CONTEXT_H_


#include "eh_config.h"
#include "eh_framework.h"


namespace cpprtl { namespace eh {

namespace x64
{
  struct nvcontext;
  extern "C" void cpprtl_eh_start_context(nvcontext const*);  // eh_context.x64.asm


  struct nvcontext
  {
    typedef enum { RIP, RBX, RBP, RSI, RDI, RSP, R12, R13, R14, R15, INT_ARRAY_SIZE } int_register;
    typedef enum { XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15, XMM_ARRAY_SIZE } xmm_register;

    ULONG64& operator[](int_register const reg)       { return int_context[reg]; }
    ULONG64  operator[](int_register const reg) const { return int_context[reg]; }

    M128A&       operator[](xmm_register const reg)       { return xmm_context[reg]; }
    M128A const& operator[](xmm_register const reg) const { return xmm_context[reg]; }

    ULONG64& IP() { return int_context[RIP]; }

    nvcontext& operator=(::CONTEXT const& ctx)
    {
      xmm_context[XMM6]  = ctx.Xmm6;  
      xmm_context[XMM7]  = ctx.Xmm7;  
      xmm_context[XMM8]  = ctx.Xmm8;  
      xmm_context[XMM9]  = ctx.Xmm9;  
      xmm_context[XMM10] = ctx.Xmm10; 
      xmm_context[XMM11] = ctx.Xmm11; 
      xmm_context[XMM12] = ctx.Xmm12; 
      xmm_context[XMM13] = ctx.Xmm13; 
      xmm_context[XMM14] = ctx.Xmm14; 
      xmm_context[XMM15] = ctx.Xmm15; 
      int_context[RIP]   = ctx.Rip;   
      int_context[RBX]   = ctx.Rbx;   
      int_context[RBP]   = ctx.Rbp;   
      int_context[RSP]   = ctx.Rsp;   
      int_context[RSI]   = ctx.Rsi;   
      int_context[RDI]   = ctx.Rdi;   
      int_context[R12]   = ctx.R12;   
      int_context[R13]   = ctx.R13;   
      int_context[R14]   = ctx.R14;   
      int_context[R15]   = ctx.R15;   
      return *this;
    }

    __declspec(noreturn)
    void start() const
    {
      cpprtl_eh_start_context(this);
    }

  private:
    M128A   xmm_context[XMM_ARRAY_SIZE];
    ULONG64 int_context[INT_ARRAY_SIZE];

    void* operator new(size_t);
    void* operator new[](size_t);
  };
}  // namespace x64

using x64::nvcontext;

}}  // namespace cpprtl::eh

#endif  // include guard

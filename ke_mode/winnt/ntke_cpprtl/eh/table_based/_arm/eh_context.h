//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_CONTEXT_H_
#define EH_CONTEXT_H_


#include "eh_config.h"
#include "eh_framework.h"


namespace cpprtl { namespace eh {

namespace arm
{
  struct nvcontext;
  extern "C" void cpprtl_eh_start_context(nvcontext const*);  // eh_context.arm.asm


  struct nvcontext
  {
    typedef enum { R4, R5, R6, R7, R8, R9, R10, R11, SP, PC, INT_ARRAY_SIZE } int_register;
    typedef enum { D8, D9, D10, D11, D12, D13, D14, D15, VFP_ARRAY_SIZE } vfp_register;

    ULONG& operator[](int_register const reg)       { return int_context[reg]; }
    ULONG  operator[](int_register const reg) const { return int_context[reg]; }

    ULONGLONG& operator[](vfp_register const reg)       { return vfp_context[reg]; }
    ULONGLONG  operator[](vfp_register const reg) const { return vfp_context[reg]; }

    ULONG& IP() { return int_context[PC]; }
    ULONG* NV() { return &int_context[R4]; }

    nvcontext& operator=(::CONTEXT const& ctx)
    {
      vfp_context[D8]    = ctx.D[8];
      vfp_context[D9]    = ctx.D[9];
      vfp_context[D10]   = ctx.D[10];
      vfp_context[D11]   = ctx.D[11];
      vfp_context[D12]   = ctx.D[12];
      vfp_context[D13]   = ctx.D[13];
      vfp_context[D14]   = ctx.D[14];
      vfp_context[D15]   = ctx.D[15];
      int_context[R4]    = ctx.R4;   
      int_context[R5]    = ctx.R5;   
      int_context[R6]    = ctx.R6;   
      int_context[R7]    = ctx.R7;   
      int_context[R8]    = ctx.R8;   
      int_context[R9]    = ctx.R9;   
      int_context[R10]   = ctx.R10;  
      int_context[R11]   = ctx.R11;  
      int_context[SP]    = ctx.Sp;   
      int_context[PC]    = ctx.Pc;   
      return *this;
    }

    __declspec(noreturn)
    void start() const
    {
      cpprtl_eh_start_context(this);
    }

  private:
    ULONGLONG vfp_context[VFP_ARRAY_SIZE];
    ULONG     int_context[INT_ARRAY_SIZE];

    void* operator new(size_t);
    void* operator new[](size_t);
  };
}  // namespace arm

using arm::nvcontext;

}}  // namespace cpprtl::eh

#endif  // include guard

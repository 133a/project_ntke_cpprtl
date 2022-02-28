//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_data.h"


namespace cpprtl { namespace eh { namespace msvc_data
{
#pragma warning(push)
#pragma warning(disable:4731)  // frame pointer register 'ebp' modified by inline assembly code

  __declspec(noreturn,naked)
  void __fastcall continuation::start(continuation_ft const continuation, size_t const frame_base, size_t const stack_pointer)
  {
    //  ecx=continuation, edx=frame_base, [esp+4]=stack_pointer
    __asm
    {
      // set up the stack frame
      mov   ebp , edx
      mov   esp , [esp+4]
      // jump to continuation
      jmp   ecx
      // just in case
      ret   4
    }
  }

  __declspec(naked)
  continuation_ft __fastcall funclet::run(funclet_ft const funclet, size_t const frame_base)
  {
    // ecx=funclet, edx=frame_base
    __asm
    {
      push  ebx
      push  edi
      push  esi

      push  ebp
      mov   ebp , edx  // set the funclet frame base
      call  ecx        // run the funclet
      pop   ebp

      pop   esi
      pop   edi
      pop   ebx
      ret
    }
    // catch block funclet returns eax=continuation
  }

#pragma warning(pop)  // disable:4731
}}}  // namespace cpprtl::eh::msvc_data

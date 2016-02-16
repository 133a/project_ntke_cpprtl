/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_msvc_internal_data.h"
#include "eh_exception_registration.h"


namespace cpprtl
{
namespace msvc_internal_data
{
namespace eh
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4731)  //  frame pointer register 'ebp' modified by inline assembly code
#endif //_MSC_VER

  continuation_ft aux_::funclet::invoke(aux_::funclet* const f, exception_registration const* const exc_reg)
  {
    ::size_t const frame_base = exc_reg->x86_ebp();
    __asm
    {
      mov   eax , f

      // modify registers by 'xor' to point the compiler for saving them at the function prolog
      // TODO check if only the x86::ecx and x86::ebp saving is necessary
      xor   esi , esi
      xor   edi , edi
      xor   ebx , ebx
      xor   ecx , ecx

      // set the frame base of the funclet and invoke the funclet
      push  ebp
      mov   ebp , frame_base
      call  eax
      pop   ebp
    }
    // return x86::eax - 'funclet* f' if the funclet being actually called has nothing to return
  }

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4731)    //  frame pointer register 'ebp' modified by inline assembly code
#endif //_MSC_VER

  void aux_::continuation::invoke(aux_::continuation* const c, exception_registration const* const exc_reg)
  {
    ::size_t const frame_base = exc_reg->x86_ebp();
    ::size_t const stack_ptr  = exc_reg->x86_esp();
    __asm
    {
      // unlinking the current exception_registration (if any) is expected to be done by the caller of this function

      // set the continuation address
      mov   eax , c

      // set up the stack frame
      mov   ebx , stack_ptr
      mov   ebp , frame_base   //  be aware after this point the current stack frame is unavailable
      mov   esp , ebx

      // jump to continuation
      jmp   eax
    }
  }

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER


}  //  namespace eh
}  //  namespace msvc_internal_data
}  //  namespace cpprtl


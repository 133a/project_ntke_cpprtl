/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"
#include "eh_engine.h"


#if !defined (_MSC_VER) || defined (__ICL)
typedef cpprtl::msvc_internal_data::eh::exception_descriptor _ThrowInfo;
#endif 


////////////////////////////////////////////////////
////
////  compiler's internally pre-declared EH routines' entry points
////
////////////////////////////////////////////////////


extern "C" void __stdcall _CxxThrowException(void* exc_object, ::_ThrowInfo const* exc_descr)
{
  cpprtl::eh::eh_engine::throw_exception
  (
    exc_object
  , reinterpret_cast<cpprtl::msvc_internal_data::eh::exception_descriptor const*>(exc_descr)
  );
}


extern "C" ::EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler3(::EXCEPTION_RECORD* exc_rec, void* frame_ptr, void* context, void* dc)
{
  cpprtl::msvc_internal_data::eh::func_descriptor* func_dsc = 0;
  __asm  mov func_dsc, eax
  return
    cpprtl::eh::eh_engine::frame_handler3
    (
      exc_rec
    , reinterpret_cast<cpprtl::eh::eh_engine::exception_registration*>(frame_ptr)
    , context
    , dc
    , func_dsc
    );
}


// __declspec(naked) 'cos it's just a jump to the frame handler 3
extern "C" __declspec(naked) ::EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler(::EXCEPTION_RECORD* exc_rec, void* frame_ptr, void* context, void* dc)
{
  __asm  jmp __CxxFrameHandler3
}


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_framework_specific_header.h"
#include "eh_engine.h"


#if !defined (_MSC_VER) || defined (__ICL)
typedef cpprtl::msvc_internal_data::eh::exception_descriptor _ThrowInfo;
#endif 


//////////////////////////////////////////////////////////
////
////  cl internally pre-declared EH routines' entry points
////
//////////////////////////////////////////////////////////

extern "C"
void _CxxThrowException(void* exc_object, ::_ThrowInfo const* exc_descr)
{
  cpprtl::eh::eh_engine::throw_exception
  (
    exc_object
  , reinterpret_cast<cpprtl::msvc_internal_data::eh::exception_descriptor const*>(exc_descr)
  );
}


extern "C"
::EXCEPTION_DISPOSITION __CxxFrameHandler3(::EXCEPTION_RECORD* exc_rec, void* frame_ptr, ::CONTEXT* context, ::DISPATCHER_CONTEXT* dc)
{
  return
    cpprtl::eh::eh_engine::frame_handler3
    (
      *exc_rec
    , reinterpret_cast<cpprtl::eh::eh_engine::frame_ptr_t>(frame_ptr)
    , *context
    , *dc
    );
}


extern "C"
::EXCEPTION_DISPOSITION __CxxFrameHandler(::EXCEPTION_RECORD* exc_rec, void* frame_ptr, ::CONTEXT* context, ::DISPATCHER_CONTEXT* dc)
{
  return
    cpprtl::eh::eh_engine::frame_handler3
    (
      *exc_rec
    , reinterpret_cast<cpprtl::eh::eh_engine::frame_ptr_t>(frame_ptr)
    , *context
    , *dc
    );
}


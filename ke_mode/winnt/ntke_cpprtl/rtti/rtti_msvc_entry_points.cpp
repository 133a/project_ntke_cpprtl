/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtti_config.h"
#include "rtti_engine.h"


////////////////////////////////////////////////////
////
////  compiler's internally pre-declared RTTI routines' entry points
////
////////////////////////////////////////////////////

extern "C" void* __cdecl __RTDynamicCast(void* src_obj, long vfdelta, void* src_type, void* dst_type, int is_ref)
throw (...)
{
  return cpprtl::rtti::dynamic_cast_impl
  (
    reinterpret_cast<cpprtl::rtti::pobj_t>(src_obj)
    , vfdelta
    , *reinterpret_cast<cpprtl::msvc_internal_data::rtti::type_descriptor*>(src_type)
    , *reinterpret_cast<cpprtl::msvc_internal_data::rtti::type_descriptor*>(dst_type)
    , 0 != is_ref
  );
}


extern "C" void* __cdecl __RTtypeid(void* src_obj)
throw (...)
{
  return cpprtl::rtti::typeid_impl(reinterpret_cast<cpprtl::rtti::pobj_t>(src_obj));
}


extern "C" void* __cdecl __RTCastToVoid(void* src_obj)
throw (...)
{
  return cpprtl::rtti::cast_to_void_impl(reinterpret_cast<cpprtl::rtti::pobj_t>(src_obj));
}


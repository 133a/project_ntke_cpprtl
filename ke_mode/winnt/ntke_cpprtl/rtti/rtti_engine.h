/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_ENGINE_H_
#define RTTI_ENGINE_H_


#include "rtti_config.h"


namespace cpprtl
{

  namespace msvc_internal_data
  {
    namespace rtti
    {
      struct type_descriptor;
    }
  }


  namespace rtti
  {
    namespace aux_ { struct cpp_obj; }
    typedef aux_::cpp_obj const*   pcobj_t;
    typedef aux_::cpp_obj*         pobj_t;


    void* dynamic_cast_impl
    (
      pcobj_t     src_obj
    , long        vfdelta
    , msvc_internal_data::rtti::type_descriptor  const&  src_type
    , msvc_internal_data::rtti::type_descriptor  const&  dst_type
    , bool        is_ref
    );

    void* typeid_impl(pcobj_t src_obj);

    void* cast_to_void_impl(pcobj_t src_obj);

  } // namespace rtti

} // namespace cpprtl


#endif // include guard


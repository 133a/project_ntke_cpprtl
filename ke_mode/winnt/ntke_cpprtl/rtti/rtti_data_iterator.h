//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTTI_DATA_ITERATOR_H_
#define RTTI_DATA_ITERATOR_H_


#include "rtti_config.h"
#include "rtti_data.h"
#include <msvc_data_iterator.h>


namespace cpprtl { namespace msvc_data_aux
{
  using namespace rtti::msvc_data;

  // class_hierarchy_accessor
  typedef accessor<direct<class_hierarchy_descriptor> > class_hierarchy_accessor;

  template <> struct descriptor2<class_hierarchy_descriptor>
  {
    static class_hierarchy_descriptor const* data(complete_object_descriptor const& complete_object, imagebase_t const image_base)
    {
    #ifndef CPPRTL_RTTI_CFG_RVA
      return complete_object.class_hierarchy;
    #else
      return rva_cast<class_hierarchy_descriptor const*>(complete_object.class_hierarchy, image_base);
    #endif
    }

    static class_hierarchy_descriptor const* data(base_class_descriptor const& base_class, imagebase_t const image_base)
    {
    #ifndef CPPRTL_RTTI_CFG_RVA
      return base_class.class_hierarchy;
    #else
      return rva_cast<class_hierarchy_descriptor const*>(base_class.class_hierarchy, image_base);
    #endif
    }
  };

  // type_descriptor_accessor
  typedef accessor<direct<type_descriptor> > type_descriptor_accessor;

  template <> struct descriptor2<type_descriptor>
  {
    static type_descriptor const* data(complete_object_descriptor const& complete_object, imagebase_t const image_base)
    {
    #ifndef CPPRTL_RTTI_CFG_RVA
      return complete_object.type_info;
    #else
      return rva_cast<type_descriptor const*>(complete_object.type_info, image_base);
    #endif
    }

    static type_descriptor const* data(base_class_descriptor const& base_class, imagebase_t const image_base)
    {
    #ifndef CPPRTL_RTTI_CFG_RVA
      return base_class.type_info;
    #else
      return rva_cast<type_descriptor const*>(base_class.type_info, image_base);
    #endif
    }
  };

  // base_class_iterator
  typedef iterator<indirect<base_class_descriptor> > base_class_iterator;

  template <> struct descriptor2<base_class_descriptor>
  {
  #ifndef CPPRTL_RTTI_CFG_RVA
    static base_class_descriptor const* const* data(class_hierarchy_descriptor const& class_hierarchy, imagebase_t)
    {
      return class_hierarchy.base_class_array->base_class_descriptor_array;
    }
  #else
    static rva_t const* data(class_hierarchy_descriptor const& class_hierarchy, imagebase_t const image_base)
    {
      return rva_cast<base_class_array_descriptor const*>(class_hierarchy.base_class_array, image_base)->base_class_descriptor_array;
    }
  #endif

    static unsigned long size(class_hierarchy_descriptor const& class_hierarchy, imagebase_t)
    {
      return class_hierarchy.base_class_array_size;
    }
  };
}}  // namespace cpprtl::msvc_data_aux


namespace cpprtl { namespace rtti { namespace msvc_data
{
  using msvc_data_aux::class_hierarchy_accessor;
  using msvc_data_aux::type_descriptor_accessor;
  using msvc_data_aux::base_class_iterator;
}}}  // namespace cpprtl::rtti::msvc_data


#endif  // include guard

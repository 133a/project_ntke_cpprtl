/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtti_config.h"
#include "rtti_msvc_internal_data_aux.h"


namespace cpprtl
{
namespace msvc_internal_data_aux
{

  //////////////////////////////////////////////////////////
  //////////  MSVC RTTI specific types information
  //////////////////////////////////////////////////////////
  // build time initialized static values

  template <>
  cl_info<msvc_internal_data::rtti::type_descriptor, msvc_internal_data::rtti::complete_object_locator>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::rtti::type_descriptor, msvc_internal_data::rtti::complete_object_locator>::base_t::addr_offset
  = &msvc_internal_data::rtti::complete_object_locator::type_descr;


  template <>
  cl_info<msvc_internal_data::rtti::class_hierarchy_descriptor, msvc_internal_data::rtti::complete_object_locator>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::rtti::class_hierarchy_descriptor, msvc_internal_data::rtti::complete_object_locator>::base_t::addr_offset
  = &msvc_internal_data::rtti::complete_object_locator::class_hierarchy_descr;


  template <>
  cl_info<msvc_internal_data::rtti::class_hierarchy_descriptor, msvc_internal_data::rtti::base_class_descriptor2>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::rtti::class_hierarchy_descriptor, msvc_internal_data::rtti::base_class_descriptor2>::base_t::addr_offset
  = &msvc_internal_data::rtti::base_class_descriptor2::class_hierarchy_descr;


  template <>
  cl_info<msvc_internal_data::rtti::base_class_array_descriptor, msvc_internal_data::rtti::class_hierarchy_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::rtti::base_class_array_descriptor, msvc_internal_data::rtti::class_hierarchy_descriptor>::base_t::addr_offset
  = &msvc_internal_data::rtti::class_hierarchy_descriptor::base_class_array;


  template <>
  cl_info<msvc_internal_data::rtti::base_class_descriptor2, msvc_internal_data::rtti::base_class_array_descriptor, msvc_internal_data::rtti::class_hierarchy_descriptor>::base_t::ARRAY_OFFSET
  cl_info<msvc_internal_data::rtti::base_class_descriptor2, msvc_internal_data::rtti::base_class_array_descriptor, msvc_internal_data::rtti::class_hierarchy_descriptor>::base_t::array_offset 
  = &msvc_internal_data::rtti::base_class_array_descriptor::base_class_descriptor_array;


  template <>
  cl_info<msvc_internal_data::rtti::base_class_descriptor2, msvc_internal_data::rtti::base_class_array_descriptor, msvc_internal_data::rtti::class_hierarchy_descriptor>::base_t::ARRAY_SIZE_OFFSET
  cl_info<msvc_internal_data::rtti::base_class_descriptor2, msvc_internal_data::rtti::base_class_array_descriptor, msvc_internal_data::rtti::class_hierarchy_descriptor>::base_t::array_size_offset
  = &msvc_internal_data::rtti::class_hierarchy_descriptor::base_class_array_size;


  template <>
  cl_info<msvc_internal_data::rtti::type_descriptor, msvc_internal_data::rtti::base_class_descriptor2>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::rtti::type_descriptor, msvc_internal_data::rtti::base_class_descriptor2>::base_t::addr_offset
  = &msvc_internal_data::rtti::base_class_descriptor2::type_descr;

}  //  namespace msvc_internal_data_aux
}  //  namespace cpprtl


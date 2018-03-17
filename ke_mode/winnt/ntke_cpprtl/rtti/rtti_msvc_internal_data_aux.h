/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_MSVC_INTERNAL_DATA_AUX_H_
#define RTTI_MSVC_INTERNAL_DATA_AUX_H_


#include "rtti_config.h"
#include "rtti_msvc_internal_data.h"

#include <msvc_internal_data_aux.h>
#include <msvc_internal_data_iterator.h>


namespace cpprtl
{
namespace msvc_internal_data_aux
{

  template <typename DST, typename A_SRC, typename SZ_SRC = A_SRC>
  struct cl_info;


  template <>
  struct cl_info
  <
    msvc_internal_data::rtti::class_hierarchy_descriptor
  , msvc_internal_data::rtti::complete_object_locator
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::class_hierarchy_descriptor
      , msvc_internal_data::rtti::complete_object_locator
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::class_hierarchy_descriptor
      , msvc_internal_data::rtti::complete_object_locator
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::rtti::class_hierarchy_descriptor
  , msvc_internal_data::rtti::base_class_descriptor2
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::class_hierarchy_descriptor
      , msvc_internal_data::rtti::base_class_descriptor2
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::class_hierarchy_descriptor
      , msvc_internal_data::rtti::base_class_descriptor2
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::rtti::type_descriptor
  , msvc_internal_data::rtti::complete_object_locator
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::type_descriptor
      , msvc_internal_data::rtti::complete_object_locator
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::type_descriptor
      , msvc_internal_data::rtti::complete_object_locator
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::rtti::base_class_array_descriptor
  , msvc_internal_data::rtti::class_hierarchy_descriptor
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::base_class_array_descriptor
      , msvc_internal_data::rtti::class_hierarchy_descriptor
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::base_class_array_descriptor
      , msvc_internal_data::rtti::class_hierarchy_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::rtti::type_descriptor
  , msvc_internal_data::rtti::base_class_descriptor2
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::type_descriptor
      , msvc_internal_data::rtti::base_class_descriptor2
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::rtti::type_descriptor
      , msvc_internal_data::rtti::base_class_descriptor2
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::rtti::base_class_descriptor2
  , msvc_internal_data::rtti::base_class_array_descriptor
  , msvc_internal_data::rtti::class_hierarchy_descriptor
  >
    : public cl_info_base_with_array_iterator
      <
        msvc_internal_data::rtti::base_class_descriptor2
      , msvc_internal_data::rtti::base_class_array_descriptor
      , msvc_internal_data::rtti::class_hierarchy_descriptor
      >
  {
    typedef cl_info_base_with_array_iterator
      <
        msvc_internal_data::rtti::base_class_descriptor2
      , msvc_internal_data::rtti::base_class_array_descriptor
      , msvc_internal_data::rtti::class_hierarchy_descriptor
      >
    base_t;
  };


}  // namespace msvc_internal_data_aux


namespace rtti
{
  // cl-rtti data type iterators

  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::class_hierarchy_descriptor
  , msvc_internal_data::rtti::complete_object_locator
  >::iterator                                                      class_hierarchy;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::class_hierarchy_descriptor
  , msvc_internal_data::rtti::base_class_descriptor2
  >::iterator                                                      base_class_hierarchy;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::type_descriptor
  , msvc_internal_data::rtti::complete_object_locator
  >::iterator                                                      complete_object_type_descriptor;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::base_class_array_descriptor
  , msvc_internal_data::rtti::class_hierarchy_descriptor
  >::iterator                                                      base_class_array;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::base_class_descriptor2
  , msvc_internal_data::rtti::base_class_array_descriptor
  , msvc_internal_data::rtti::class_hierarchy_descriptor
  >::fwd_iterator                                                  base_class_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::base_class_descriptor2
  , msvc_internal_data::rtti::base_class_array_descriptor
  , msvc_internal_data::rtti::class_hierarchy_descriptor
  >::rev_iterator                                                  base_class_rev_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::rtti::type_descriptor
  , msvc_internal_data::rtti::base_class_descriptor2
  >::iterator                                                      base_class_type_descriptor;

}  // namespace rtti
}  // namespace cpprtl


#endif  // include guard


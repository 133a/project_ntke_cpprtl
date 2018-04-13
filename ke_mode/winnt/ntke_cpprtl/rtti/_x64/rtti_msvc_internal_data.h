/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_MSVC_INTERNAL_DATA_H_
#define RTTI_MSVC_INTERNAL_DATA_H_


#include "rtti_config.h"
#include "rtti_msvc_internal_data_common.h"


namespace cpprtl
{
namespace msvc_internal_data
{

////////////////////////////////////////////////////////
////////  MSVC compiler internal stuff x64
////////////////////////////////////////////////////////
namespace rtti
{

  struct base_class_descriptor2
  {
    rva_t              type_descr;             // type descriptor of the class 'type_info' (-->type_descriptor*)
    unsigned long      num_contained_bases;    // number of nested classes following in the base_class_array
    subtype_cast_info  cast_info;              // pointer-to-member displacement info
    unsigned long      attributes;             // attributes
    rva_t              class_hierarchy_descr;  // available if BC_WITH_HIERARCHY is set (-->class_hierarchy_descriptor*)
  };

  
  struct base_class_array_descriptor
  {
    rva_t base_class_descriptor_array[1];  // array of rva_t (-->base_class_descriptor*), [1] is a fictive size
  };

  
  struct class_hierarchy_descriptor
  {
    unsigned long  signature;              // always zero ?
    unsigned long  attributes;             // bit 0 set: multiple inheritance, bit 1 set: virtual inheritance
    unsigned long  base_class_array_size;  // number of classes in base_class_array
    rva_t          base_class_array;       // (--> base_class_array_descriptor*)
  };

  
  struct complete_object_locator
  {
    unsigned long  signature;              // always zero ?
    unsigned long  offset;                 // offset of this vtable in the complete class
    unsigned long  cd_offset;              // construction displacement offset
    rva_t          type_descr;             // type_descriptor of the complete class (--> type_descriptor*)
    rva_t          class_hierarchy_descr;  // describes inheritance hierarchy (--> class_hierarchy_descriptor*)
  };

}  // namespace rtti

}  // namespace msvc_internal_data
}  // namespace cpprtl


#endif  // include guard


//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTTI_DATA_H_
#define RTTI_DATA_H_


#include "rtti_config.h"


namespace cpprtl { namespace rtti { namespace msvc_data
{
//====================
// msvc internal stuff 
//--------------------

  // complete_object_descriptor.signature
  enum
  {
    CO_SIG_V0 = 0
  , CO_SIG_V1 = 1
  };

  // class_hierarchy_descriptor.attributes
  enum
  {
    CH_MULTIPLE_INHERITANCE  = 1 << 0
  , CH_VIRTUAL_INHERITANCE   = 1 << 1
  , CH_AMBIGUOUS             = 1 << 2
  };

  // base_class_descriptor.attributes
  // BC_WITH_HIERARCHY is supported since msvc2005, is not supported by msvc2003, ddk2600, ddk3790 and earlier toolsets
  enum
  {
    BC_NOT_VISIBLE       = 1 << 0
  , BC_AMBIGUOUS         = 1 << 1
  , BC_NOT_PUBLIC_BASE_4 = 1 << 2  // TODO >= msvc2005 use this flag
  , BC_NOT_PUBLIC_BASE_8 = 1 << 3  // TODO all toolsets
  , BC_0x10              = 1 << 4
  , BC_0x20              = 1 << 5
  , BC_WITH_HIERARCHY    = 1 << 6
  };


  // msvc ::type_info raw look-alike
  struct type_descriptor
  {
    void const*  vtable;          // vtable of type_info class
    char*        unmangled_name;  // used by msvcrt to keep the demangled name returned by type_info::name()
    char const   name[1];         // [VSA], keeps a zero-terminated mangled type name
  };

  struct subtype_cast_info
  {
    int  subtype_offset;      // member offset
    int  vbase_table_offset;  // offset of the vbtable (-1 if not a virtual base)
    int  vbase_disp_offset;   // offset to the displacement value inside the vbtable
  };

#ifndef CPPRTL_RTTI_CFG_RVA
  struct class_hierarchy_descriptor;

  struct base_class_descriptor
  {
    type_descriptor*             type_info;        // base class type_info
    unsigned long                nested_bases;     // number of nested classes following in the base_class_array
    subtype_cast_info            cast_info;        // pointer-to-member displacement info
    unsigned long                attributes;       // BC_ enum
    class_hierarchy_descriptor*  class_hierarchy;  // available if BC_WITH_HIERARCHY is set
  };

  struct base_class_array_descriptor
  {
    base_class_descriptor*  base_class_descriptor_array[1];  // [VSA] of base_class_descriptor*
  };
  
  struct class_hierarchy_descriptor
  {
    unsigned long                 signature;              // zero
    unsigned long                 attributes;             // CH_ enum
    unsigned long                 base_class_array_size;  // number of classes in base_class_array
    base_class_array_descriptor*  base_class_array;
  };
  
  struct complete_object_descriptor
  {
    unsigned long                signature;        // CO_ enum
    unsigned long                offset;           // vtable offset in the complete class
    unsigned long                cd_offset;        // construction displacement offset
    type_descriptor*             type_info;        // complete class type_info
    class_hierarchy_descriptor*  class_hierarchy;  // inheritance hierarchy
    complete_object_descriptor*  self;             // >=CO_SIG_V1, this pointer
  };
#else
  struct base_class_descriptor
  {
    rva_t              type_info;        // base class type_info (-->type_descriptor*)
    unsigned long      nested_bases;     // number of nested classes following in the base_class_array
    subtype_cast_info  cast_info;        // pointer-to-member displacement info
    unsigned long      attributes;       // BC_ enum
    rva_t              class_hierarchy;  // available if BC_WITH_HIERARCHY is set (-->class_hierarchy_descriptor*)
  };
  
  struct base_class_array_descriptor
  {
    rva_t base_class_descriptor_array[1];  // [VSA] of -->base_class_descriptor*
  };
  
  struct class_hierarchy_descriptor
  {
    unsigned long  signature;              // zero
    unsigned long  attributes;             // CH_ enum
    unsigned long  base_class_array_size;  // number of classes in base_class_array
    rva_t          base_class_array;       // (-->base_class_array_descriptor*)
  };
  
  struct complete_object_descriptor
  {
    unsigned long  signature;        // CO_ enum
    unsigned long  offset;           // vtable offset in the complete class
    unsigned long  cd_offset;        // construction displacement offset
    rva_t          type_info;        // complete class type_info (-->type_descriptor*)
    rva_t          class_hierarchy;  // inheritance hierarchy (-->class_hierarchy_descriptor*)
    rva_t          self;             // >=CO_SIG_V1, rva(this) (-->complete_object_descriptor*)
  };
#endif  // CPPRTL_RTTI_CFG_RVA

}}}  // namespace cpprtl::rtti::msvc_data


#endif  // include guard

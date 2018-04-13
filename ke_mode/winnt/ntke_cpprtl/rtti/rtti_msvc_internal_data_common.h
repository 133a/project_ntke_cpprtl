/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_MSVC_INTERNAL_DATA_COMMON_H_
#define RTTI_MSVC_INTERNAL_DATA_COMMON_H_


#include "rtti_config.h"


namespace cpprtl
{
namespace msvc_internal_data
{

////////////////////////////////////////////////////////
////////  MSVC compiler internal stuff 
////////////////////////////////////////////////////////
namespace rtti
{

  ////  ::type_info raw look-alike
  struct type_descriptor
  {
    void const*  vtable_ptr;      // vtable of type_info class
    char*        unmangled_name;  // used by msvcrt to keep the demangled name returned by type_info::name()
    char         name;            // the actual type is char[], keeps a zero-terminated mangled type name, e.g. ".H" = "int", ".?AUA@@" = "struct A", ".?AVA@@" = "class A"
  };


  struct subtype_cast_info
  {
    int  subtype_offset;      // member offset
    int  vbase_table_offset;  // offset of the vbtable (-1 if not a virtual base)
    int  vbase_disp_offset;   // offset to the displacement value inside the vbtable
  };


  // base_class_descriptor.attributes flags
  enum
  {
    BC_NOT_VISIBLE      = 1 << 0
  , BC_AMBIGUOUS        = 1 << 1
  , BC_TODO_0x04        = 1 << 2            
  , BC_NOT_PUBLIC_BASE  = 1 << 3
  , BC_TODO_0x10        = 1 << 4            
  , BC_NOT_POLYMORPHIC  = 1 << 5
  , BC_WITH_HIERARCHY   = 1 << 6
  };
//  BC_WITH_HIERARCHY support is started since cl.exe-14.00.50727 (msvc2005)
//  BC_WITH_HIERARCHY ISN't supported by:
//    msvc2003  ddk2600  ddk3790sp1
//  BC_WITH_HIERARCHY IS supported by:
//    msvc2005  msvc2008  msvc2010  msvc2012  msvc2013  msvc2015  msvc2017 winsdk7.1 ewdk16299-um
//    wdk7.1.0  wdk8.0  wdk8.1  wdk10 ewdk16299-km


  // class_hierarchy_descriptor.attributes flags
  enum
  {
    CH_MULTIPLE_INHERITANCE  = 1 << 0
  , CH_VIRTUAL_INHERITANCE   = 1 << 1
  , CH_AMBIGUOUS             = 1 << 2
  };

}  // namespace rtti
}  // namespace msvc_internal_data
}  // namespace cpprtl


#endif  // include guard


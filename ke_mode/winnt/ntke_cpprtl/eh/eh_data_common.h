//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_DATA_COMMON_H_
#define EH_DATA_COMMON_H_


#include "eh_config.h"


namespace cpprtl { namespace eh { namespace msvc_data
{
//====================
// msvc internal stuff
//--------------------

  // exception_object_descriptor.attributes
  enum
  {
    EXCOBJECT_CONST      = 1 << 0
  , EXCOBJECT_VOLATILE   = 1 << 1
  , EXCOBJECT_UNALIGNED  = 1 << 2
  };

  // catchable_type_descriptor.attributes
  enum
  {
    EXCOBJECT_SIMPLE_TYPE       = 1 << 0
  , EXCOBJECT_REFERENCE         = 1 << 1
  , EXCOBJECT_HAS_VIRTUAL_BASE  = 1 << 2
  };

  // catch_block_descriptor.attributes
  enum
  {
    CATCH_CONST      = 1 << 0
  , CATCH_VOLATILE   = 1 << 1
  , CATCH_UNALIGNED  = 1 << 2
  , CATCH_REFERENCE  = 1 << 3
  , CATCH_ELLIPSIS   = 1 << 6
  };

  // function_descriptor.signature
  // 0x19930520: <= msvc6, 0x19930521: msvc2002 msvc2003, 0x19930522: >=msvc2005
  enum
  {
    MARK_VC6 = 0x19930520
  , MARK_VC7 = 0x19930521
  , MARK_VC8 = 0x19930522
  };

  // function_descriptor.flags, function_descriptor.signature >= MARK_VC8
  enum
  {
    FUNCTION_EHs      = 1 << 0  // the function was compiled with -EHs switch
  , FUNCTION_NOEXCEPT = 1 << 2  // the function has 'noexcept(true)' specificator, >=C++11
  };


  // stack frame state
  typedef int frame_state_t;
  namespace frame_state
  {
    enum { INVALID = -2, EMPTY = -1 };
  }


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

}}}  // namespace cpprtl::eh::msvc_data


namespace cpprtl { namespace eh
{
  inline size_t pointer_cast(size_t ptr, msvc_data::subtype_cast_info const& cast_info)
  {
    if ( cast_info.vbase_table_offset >= 0 )
    {
      ptr += cast_info.vbase_table_offset;
      ptr += *reinterpret_cast<int const*>(*reinterpret_cast<size_t const*>(ptr) + cast_info.vbase_disp_offset);
    }
    ptr += cast_info.subtype_offset;
    return ptr;
  }
}}  // namespace cpprtl::eh


#endif  // include guard

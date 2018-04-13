/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_MSVC_INTERNAL_DATA_CONST_H_
#define EH_MSVC_INTERNAL_DATA_CONST_H_


#include "eh_config.h"


namespace cpprtl
{
namespace msvc_internal_data
{

////////////////////////////////////////////////////////
////////  MSVC compiler internal stuff
////////////////////////////////////////////////////////
namespace eh
{

  // catchable_type->attributes
  enum
  {
    EXCEP_SIMPLE_TYPE   = 1 << 0
  , EXCEP_REFERENCE     = 1 << 1
  , EXCEP_VIRTUAL_BASE  = 1 << 2
  };


  // exception_descriptor->attributes
  enum
  {
    EXCEP_CONST     = 1 << 0
  , EXCEP_VOLATILE  = 1 << 1
  };


  // catch_descriptor->attributes
  enum
  {
    CATCH_CONST      = 1 << 0
  , CATCH_VOLATILE   = 1 << 1
  , CATCH_UNALIGNED  = 1 << 2
  , CATCH_REFERENCE  = 1 << 3
  };


  // func_descriptor->magic_number
  enum
  {
    EH_VC6 = 0x19930520
  , EH_VC7 = 0x19930521
  , EH_VC8 = 0x19930522
  };


  // func_descriptor->flags
  enum
  {
    FLAG_EHs = 1 << 0  // function was compiled with /EHs - so cpp-frame-handler provides no action for seh exceptions (VC8 +)
  };

}  // namespace eh
}  // namespace msvc_internal_data
}  // namespace cpprtl


#endif  // include guard


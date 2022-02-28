//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_ENGINE_H_
#define EH_ENGINE_H_


#include "eh_config.h"
#include <check_ct.h>


namespace cpprtl { namespace eh
{
//========================
// EXCEPTION_RECORD layout
//------------------------

  // exception flags at the ExceptionInformation[EXCDATA_FLAGS]
  enum
  {
    EXCEPTION_FLAG_NO_EXCEPTION_OBJECT = 1 << 0
  , EXCEPTION_FLAG_OBJECT_RETHROWN     = 1 << 1
  , EXCEPTION_FLAG_OBJECT_DESTRUCTED   = 1 << 2
  };


  // ExceptionInformation[] layout
  enum 
  {
    EXCDATA_FLAGS                         // EXCEPTION_FLAG_xxx
  , EXCDATA_THROW_OBJECT                  // exception object address
  , EXCDATA_THROW_INFO                    // exception_object_descriptor*
  , EXCDATA_UW_CPP_REGISTRATION           // cpp_registration*
  , EXCDATA_UW_FUNCTION_DESCRIPTOR        // function_descriptor*
  , EXCDATA_UW_TRY_BLOCK_DESCRIPTOR       // try_block_descriptor*
  , EXCDATA_UW_CATCH_BLOCK_DESCRIPTOR     // catch_block_descriptor*
  , EXCDATA_UW_CATCHABLE_TYPE_DESCRIPTOR  // catchable_type_descriptor*, unused
  , EXCDATA_UW_CATCH_DEPTH                // int
  , EXCDATA_UW_TARGET_SEH_REGISTRATION    // seh::registration*
  , ARRAYSIZE_EXCDATA_EH
  };
  COMPILE_TIME_CHECK (ARRAYSIZE_EXCDATA_EH <= EXCEPTION_MAXIMUM_PARAMETERS, EXCEPTION_INFORMATION_MAXIMUM_PARAMETERS_EXCEEDED)

}}  // namespace cpprtl::eh


#endif  // include guard

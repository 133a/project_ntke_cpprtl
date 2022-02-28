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
  , EXCEPTION_FLAG_FH4                 = 1 << 3
  , EXCEPTION_FLAG_FH4_TRANSIENT_STATE = 1 << 4
  };

  // ExceptionInformation[] layout
  // throw data, common
  enum 
  {
    EXCDATA_FLAGS            // EXCEPTION_FLAG_ enum
  , EXCDATA_THROW_OBJECT     // exception object address
  , EXCDATA_THROW_INFO       // exception_object_descriptor*
  , EXCDATA_THROW_IMAGEBASE  // imagebase_t
  , EXCDATA_CATCH_IMAGEBASE  // imagebase_t
  , EXCDATA_FUNCTION_FRAME   // function_frame_t
  , EXCDATA_TARGET_FRAME     // funclet_frame_t
  , EXCDATA_TARGET_STATE     // frame_state_t
  , EXCDATA_RUN_CATCH_BLOCK  // run_catch_block() address, size_t(*)(EXCEPTION_RECORD&)
  , EXCDATA_NVCONTEXT        // arm/arm64 specific: NV-context
  , ARRAYSIZE_EXCDATA_EH
  };

  // FH3
  enum 
  {
    EXCDATA_FH3_FUNCTION_DESCRIPTOR = ARRAYSIZE_EXCDATA_EH  // function_descriptor*
  , EXCDATA_FH3_CATCH_FUNCLET  // funclet_ft
  , ARRAYSIZE_EXCDATA_FH3
  };
  COMPILE_TIME_CHECK (ARRAYSIZE_EXCDATA_FH3 <= EXCEPTION_MAXIMUM_PARAMETERS , EXCEPTION_INFORMATION_FH3_MAXIMUM_PARAMETERS_EXCEEDED)

  // FH4
  enum 
  {
    EXCDATA_FH4_CATCH_BLOCK_DATA = ARRAYSIZE_EXCDATA_EH  // fh4::catch_block_data_packed
  , EXCDATA_FH4_FUNCLET_ENTRY    // rva_t
  , EXCDATA_FH4_CATCH_LEVEL      // frame_state_t
  , EXCDATA_FH4_TRANSIENT_STATE  // frame_state_t
  , EXCDATA_FH4_UNWIND_STATE     // frame_state_t + FRAME_STATE_SHIFT (to keep INVALID and EMPTY values as unsigned)
  , ARRAYSIZE_EXCDATA_FH4
  };
  COMPILE_TIME_CHECK (ARRAYSIZE_EXCDATA_FH4 <= EXCEPTION_MAXIMUM_PARAMETERS , EXCEPTION_INFORMATION_FH4_MAXIMUM_PARAMETERS_EXCEEDED)

}}  // namespace cpprtl::eh


#endif  // include guard

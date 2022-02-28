//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_AUX_H_
#define EH_AUX_H_


#include "eh_config.h"
#include "eh_framework.h"
#include "eh_exception_code.h"


namespace cpprtl { namespace eh { namespace aux_
{

  // memcpy
  void memcpy(void* const d, void const* const s, size_t sz);

  // compare zero-terminated ansi-strings
  bool strzeq(char const* ch1, char const* ch2);

  // terminate the process execution if the exception handling goes a wrong way
  __declspec(noreturn) void terminate();
  __declspec(noreturn) void terminate(int const exc_code, int const exc_subcode = EXCEPTION_SUBCODE_DEFAULT);

}}}  // namespace cpprtl::eh::aux_


#endif  // include guard

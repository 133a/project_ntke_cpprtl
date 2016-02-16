/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_AUX_H_
#define EH_AUX_H_


#include "eh_config.h"
#include "eh_framework_specific_header.h"
#include "eh_exception_code.h"


namespace cpprtl
{
namespace eh
{
namespace aux_
{

////   memcpy
  void memcpy(void* const d, void const* const s, ::size_t sz);

////   compare zero-terminated ansi-strings
  bool strzcmp(char const* ch1, char const* ch2);

////   terminate the process execution if the exception handling goes a wrong way
  void invalid_exception();
  void invalid_exception(int const& exc_code, int const& exc_subcode = EXCEPTION_SUBCODE_DEFAULT);

////   raise exception
  void raise_exception(::EXCEPTION_RECORD&);

}  //  namespace aux_
}  //  namespace eh
}  //  namespace cpprtl


#endif // include guard


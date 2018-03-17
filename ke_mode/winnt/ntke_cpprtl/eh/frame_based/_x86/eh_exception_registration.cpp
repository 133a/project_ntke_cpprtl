/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_exception_registration.h"
#include <check_ct.h>


namespace cpprtl
{
namespace msvc_internal_data
{
namespace eh
{

  exception_registration const* const exception_registration::REG_END = reinterpret_cast<exception_registration*>(::size_t(-1));


  COMPILE_TIME_CHECK (sizeof(exception_registration) == exception_registration::FRAME_BASE_DELTA , SIZEOF_EXCEPTION_REGISTRATION_WRONG_VALUE)

}  // namespace eh
}  // namespace msvc_internal_data
}  // namespace cpprtl



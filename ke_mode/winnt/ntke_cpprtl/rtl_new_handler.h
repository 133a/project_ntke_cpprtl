//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTL_NEW_HANDLER_H_
#define RTL_NEW_HANDLER_H_


#include "std_incl/new"


namespace cpprtl { namespace new_handler
{
  std::new_handler get();
  std::new_handler set(std::new_handler new_p);
}}  // namespace cpprtl::new_handler


#endif

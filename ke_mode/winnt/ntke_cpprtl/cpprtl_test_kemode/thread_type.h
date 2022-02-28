//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef THREAD_TYPE_H_
#define THREAD_TYPE_H_


#ifdef NTKE_LIB
#  include <ntke/kthread.h>
#else
#  include "aux_thread.h"
#endif


namespace cpprtl_tests
{
  //  choose threads to be used
#ifdef NTKE_LIB
  typedef ntke::kthread thread_type;
#else
  typedef aux_::kthread thread_type;
#endif

}  // namespace cpprtl_tests


#endif  // include guard


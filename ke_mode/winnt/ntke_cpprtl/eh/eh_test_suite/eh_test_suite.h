//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_TEST_SUITE_H_
#define EH_TEST_SUITE_H_


namespace cpprtl { namespace eh { namespace test
{
  int run_thread_safe();
  int run_thread_unsafe();
  int run_once();
}}}  // namespace cpprtl::eh::test


#endif  // include guard

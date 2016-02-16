/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_TEST_SUITE_H_
#define EH_TEST_SUITE_H_


namespace cpprtl {  namespace test  {  namespace eh
{
  enum { OK = 0 };

  int run_thread_safe();
  int run_thread_unsafe();

} } }


#endif // include guard


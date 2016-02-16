/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TEST_THREAD_H_
#define TEST_THREAD_H_


#include "tests_aux.h"


namespace cpprtl_tests
{
  enum
  {
    // RTL tests are separated by a thread-safety so you can set any reasonable number of kernel threads to spawn thread-safe tests
    THR_NUM = 64
  };

  int test_thread(testFT []);
}


#endif // include guard


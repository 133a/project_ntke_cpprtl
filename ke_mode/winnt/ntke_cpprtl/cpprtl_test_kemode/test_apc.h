/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TEST_APC_H_
#define TEST_APC_H_


#include "tests_aux.h"


namespace cpprtl_tests
{
  enum
  {
    APC_NUM = 4  // don't be allured because of the task array is placed on the stack
  };

  int test_apc    (testFT []);
  int test_apc_mt (testFT []);
}


#endif // include guard


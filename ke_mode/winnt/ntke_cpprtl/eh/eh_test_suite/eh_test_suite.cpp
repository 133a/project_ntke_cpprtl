/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_test_suite.h"
#include "tests.h"
#include "../eh_cfg_common.h"


namespace
{
  enum
  {
    IDX_FACTOR = 10000000
  };


  using namespace cpprtl::test::eh;


//// thread-safe tests
  test_ft* const test_table_thread_safe[] =
  {
    0

    , &test00
    , &test01
    , &test02
    , &test03
    , &test04
    , &test05
    , &test06
    , &test07
    , &test08
    , &test09
    , &test10
    , &test11
    , &test12
    , &test13
    , &test14
    , &test15
    , &test16
    , &test17
    , &test18
    , &test19
    , &test20
    , &test21
    , &test22
    , &test23
    , &test24
    , &test25
    , &test26

  //  , &test27
  //  , &test28
  //  , &test29


#if !defined (NT_KERNEL_MODE) || !defined (NTKE_EH_CONFIG_DO_NOT_USE_STACK_WALKER)  // stack-consuming tests are meant
    , &test100
  //  , &test101
  //  , &test102
  //  , &test103
  //  , &test104
  //  , &test105
  //  , &test106
  //  , &test107
  //  , &test108
  //  , &test109
#endif 
    , 0
  };


  unsigned test_num_thread_safe()
  {
    return sizeof(test_table_thread_safe) / sizeof(test_table_thread_safe[0]);
  }


//// thread-unsafe tests
  test_ft* const test_table_thread_unsafe[] =
  {
    0

    , &test51  // tests 51-54 are using a static context for
    , &test52  // default ctors counting while an array initialization.
    , &test53  // ^
    , &test54  // ^

  //  , &test55
  //  , &test56
  //  , &test57
  //  , &test58
  //  , &test59

    , 0
  };


  unsigned test_num_thread_unsafe()
  {
    return sizeof(test_table_thread_unsafe) / sizeof(test_table_thread_unsafe[0]);
  }


//// run loop
  int run(test_ft* const test_table[], unsigned const& test_num)
  {
    unsigned idx = 0;
    int res = 0;
    for ( ; idx < test_num ; ++idx )
    {
      if ( test_table[idx] )
      {
        if ( 0 != (res = test_table[idx]()) )
        {
          break;
        }
      }
    }
    return res ? idx * IDX_FACTOR + res : 0;
  }

}  // namespace 


namespace cpprtl  { namespace test  { namespace eh
{

  int run_thread_safe()
  {
    return run(test_table_thread_safe, test_num_thread_safe());
  }


  int run_thread_unsafe()
  {
    return run(test_table_thread_unsafe, test_num_thread_unsafe());
  }

}  }  }


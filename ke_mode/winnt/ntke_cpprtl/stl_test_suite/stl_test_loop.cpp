//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "stl_test_loop.h"
#include "stl_tests.h"


namespace cpprtl { namespace stl { namespace test
{
  namespace
  {
    test_ft const test_table_thread_safe[] =
    {
      0  // keep first
    , test_01, test_02, test_03, test_04, test_05, test_06
    };

    unsigned test_num_thread_safe()
    {
      return sizeof(test_table_thread_safe) / sizeof(test_table_thread_safe[0]);
    }
  }  // namespace


  int run()
  {
    for ( unsigned idx = 0; idx < test_num_thread_safe() ; ++idx )
    {
      if ( test_table_thread_safe[idx] )
      {
        try
        {
          if ( !test_table_thread_safe[idx]() )
          {
            return idx;
          }
        }
        catch (...)
        {
          return idx;
        }
      }
    }
    return 0;
  }
}}}  // namespace cpprtl::stl::test

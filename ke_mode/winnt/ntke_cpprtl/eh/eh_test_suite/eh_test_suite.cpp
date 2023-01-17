//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_test_suite.h"
#include "eh_tests.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    // thread-safe tests
    test_ft const test_table_thread_safe[] =
    {
      0  // keep 1st
    , test_0101, test_0102, test_0103, test_0104, test_0105, test_0106, test_0107, test_0108
    , test_0201
    , test_0301, test_0302, test_0303, test_0304
    , test_0401, test_0402, test_0403, test_0404, test_0405, test_0406, test_0407, test_0408
    , test_0501, test_0502, test_0503, test_0504, test_0505, test_0506, test_0507, test_0508, test_0509, test_0510
    , test_0601
    , test_0701, test_0702, test_0703, test_0704
    , test_0801, test_0802, test_0803, test_0804, test_0805, test_0806
    , test_0901, test_0902
    };

    // thread-unsafe tests
    test_ft const test_table_thread_unsafe[] =
    {
      0  // keep 1st
    , test_5101, test_5102, test_5103, test_5104, test_5105
    };

    test_ft const test_table_run_once[] =
    {
      0  // keep 1st
    , test_5201
    };

    // run loop
    template <unsigned LEN>
    unsigned run(test_ft const (&test_table)[LEN])
    {
      for ( unsigned idx = 0; idx < LEN; ++idx )
      {
        if ( test_table[idx] )
        {
          try
          {
            if ( !test_table[idx]() )
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
  }  // namespace 

  int run_thread_safe()
  {
    return run(test_table_thread_safe);
  }
  int run_thread_unsafe()
  {
    return run(test_table_thread_unsafe);
  }
  int run_once()
  {
    return run(test_table_run_once);
  }

}}}  // namespace cpprtl::eh::test

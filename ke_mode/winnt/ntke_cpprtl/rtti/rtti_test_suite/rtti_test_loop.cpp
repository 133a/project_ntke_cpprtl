//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtti_test_loop.h"
#include "rtti_tests.h"


namespace cpprtl { namespace rtti { namespace test
{
  namespace
  {
    test_ft const test_table_thread_safe[] =
    {
      0  // keep first

    , test_s0101, test_s0102, test_s0103, test_s0104, test_s0105, test_s0106, test_s0107, test_s0108

    , test_m0101, test_m0102, test_m0103, test_m0104
    , test_m0201, test_m0202, test_m0203, test_m0204, test_m0205, test_m0206, test_m0207
    , test_m0301, test_m0302, test_m0303, test_m0304, test_m0305, test_m0306, test_m0307, test_m0308

    , test_v0101, test_v0102, test_v0103, test_v0104
    , test_v0201, test_v0202, test_v0203, test_v0204, test_v0205
    , test_v0301, test_v0302, test_v0303, test_v0304, test_v0305, test_v0306
    , test_v0401, test_v0402, test_v0403, test_v0404, test_v0405
    , test_v0501, test_v0502, test_v0503, test_v0504, test_v0505, test_v0506

    , test_b0101, test_b0102, test_b0103
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

}}}  // namespace cpprtl::rtti::test

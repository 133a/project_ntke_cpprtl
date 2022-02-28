//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "gstatic_test_loop.h"
#include "gstatic_tests.h"


namespace cpprtl { namespace gstatic { namespace test
{
  namespace
  {
    test_ft const test_table_run_once[] =
    {
      0  // keep 1st
    , test_01, test_02, test_03
    };

    unsigned test_num_run_once()
    {
      return sizeof(test_table_run_once) / sizeof(test_table_run_once[0]);
    }
  }  // namespace


  int run_once()
  {
    for ( unsigned idx = 0; idx < test_num_run_once() ; ++idx )
    {
      if ( test_table_run_once[idx] )
      {
        if ( !test_table_run_once[idx]() )
        {
          return idx;
        }
      }
    }
    return 0;
  }

}}}  // namespace cpprtl::gstatic::test

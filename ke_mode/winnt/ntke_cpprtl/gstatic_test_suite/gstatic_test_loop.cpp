/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "gstatic_test_loop.h"
#include "tests.h"


namespace
{
  enum
  {
    IDX_FACTOR = 10000000
  };


  using namespace cpprtl::test::gstatic;


  test_ft* const test_table_thread_unsafe[] =
  {
    0

    , &test_gstatic01
    , &test_gstatic02
    , &test_gstatic03

    , 0
  };


  unsigned test_num_thread_unsafe()
  {
    return sizeof(test_table_thread_unsafe) / sizeof(test_table_thread_unsafe[0]);
  }

}  // namespace


namespace cpprtl  { namespace test  { namespace gstatic
{

  int run()
  {
    unsigned idx = 0;
    unsigned res = 0;
    for ( ; idx < test_num_thread_unsafe() ; ++idx )
    {
      if ( test_table_thread_unsafe[idx] )
      {
        if ( 0 != (res = test_table_thread_unsafe[idx]()) )
        {
          break;
        }
      }
    }
    return res ? idx * IDX_FACTOR + res : 0;
  }

}  }  }


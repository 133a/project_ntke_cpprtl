/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtti_test_loop.h"
#include "tests.h"


namespace
{
  using namespace cpprtl::test::rtti;


  test_ft* test_table_thread_safe[] =
  {
    0

    , &test_si01
  //  , &test_si02
  //  , &test_si03
  //  , &test_si04
  //  , &test_si05
  //  , &test_si06
  //  , &test_si07
  //  , &test_si08
  //  , &test_si09

    , &test_mi01
    , &test_mi02
    , &test_mi03
  //  , &test_mi04
  //  , &test_mi05
  //  , &test_mi06
  //  , &test_mi07
  //  , &test_mi08
  //  , &test_mi09

    , &test_vi01
    , &test_vi02
    , &test_vi03
    , &test_vi04
    , &test_vi05
  //  , &test_vi06
  //  , &test_vi07
  //  , &test_vi08
  //  , &test_vi09

    , 0
  };


  unsigned test_num_thread_safe()
  {
    return sizeof(test_table_thread_safe) / sizeof(test_table_thread_safe[0]);
  }

}  // namespace


namespace cpprtl  { namespace test  { namespace rtti
{

  int run_thread_safe()
  {
    unsigned idx = 0;
    unsigned res = 0;
    for ( ; idx < test_num_thread_safe() ; ++idx )
    {
      if ( test_table_thread_safe[idx] )
      {
        if ( 0 != (res = test_table_thread_safe[idx]()) )
        {
          break;
        }
      }
    }
    return res ? idx : 0;
  }

}  }  }


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include <rtl_framework_specific_header.h>


namespace
{
  enum
  {
    TEST_001 = 6767,
    TEST_002,
    TEST_003,
  };


  int ftest_001(int&);
  int ftest_002(int&);
  int ftest_003(int&);

  int res = 0;
}


int test_001          = ftest_001(res);
static int test_002   = ftest_002(res);
namespace
{
  int test_003        = ftest_003(res);
}


namespace
{

  int ftest_001(int& r)
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("test_gstatic01 ---> ftest_001\n");
  #endif
    r += TEST_001;
    return TEST_001;
  }


  int ftest_002(int& r)
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("test_gstatic01 ---> ftest_002\n");
  #endif
    r += TEST_002;
    return TEST_002;
  }


  int ftest_003(int& r)
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("test_gstatic01 ---> ftest_003\n");
  #endif
    r += TEST_003;
    return TEST_003;
  }

}


namespace cpprtl { namespace test { namespace gstatic
{

  int test_gstatic01()
  {
    return res - test_001 - test_002 - test_003 ;
  }

}  }  }


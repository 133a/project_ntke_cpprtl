/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifdef NT_KERNEL_MODE
#  include "ntddk.include.h"
#endif


namespace
{
  enum
  {
    TEST_101 = 101
  , TEST_102
  , TEST_103
  };


  int ftest_101(int&);
  int ftest_102(int&);
  int ftest_103(int&);

  int res = 0;
}


int test_101         = ftest_101(res);
static int test_102  = ftest_102(res);
namespace
{
  int test_103       = ftest_103(res);
}


namespace
{
  int ftest_101(int& r)
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("test_gstatic01 ---> ftest_101\n");
  #endif
    r += TEST_101;
    return TEST_101;
  }


  int ftest_102(int& r)
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("test_gstatic01 ---> ftest_102\n");
  #endif
    r += TEST_102;
    return TEST_102;
  }


  int ftest_103(int& r)
  {
  #ifdef NT_KERNEL_MODE
    DbgPrint("test_gstatic01 ---> ftest_103\n");
  #endif
    r += TEST_103;
    return TEST_103;
  }
}


namespace cpprtl { namespace test { namespace gstatic
{
  int test_gstatic01()
  {
    return res - test_101 - test_102 - test_103 ;
  }
}  }  }


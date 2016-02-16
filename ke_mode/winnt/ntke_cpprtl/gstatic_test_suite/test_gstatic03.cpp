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
    TEST_LOOP = 67,
    TEST_CTOR,
    TEST_DTOR,
    TEST_NUM,
  };

  int res = 0;
  int const loop_check = TEST_LOOP * TEST_NUM ;

}


namespace
{
  class ctest
  {
  public:
    int test_num;

    ctest()
      : test_num(TEST_NUM)
    {
  #ifdef NT_KERNEL_MODE
      DbgPrint("test_gstatic03 ---> ctest::ctest()\n");
  #endif
      res += TEST_CTOR;
    }

    ~ctest()
    {
  #ifdef NT_KERNEL_MODE
      DbgPrint("test_gstatic03 ---> ctest::~ctest()\n");
  #endif
    }
  };


  int func_with_static_ctest()
  {
    static ctest t000;
    res += t000.test_num;
    return t000.test_num;
  }


  int test_loop(int tl)
  {
    int ret = 0;
    for (int i = 0; i < tl; ++i)
    {
      ret += func_with_static_ctest();
    }
    return ret;
  }
}


namespace cpprtl { namespace test { namespace gstatic
{

  int test_gstatic03()
  {
    test_loop(TEST_LOOP);
    return res - TEST_CTOR - loop_check;
  }

}  }  }


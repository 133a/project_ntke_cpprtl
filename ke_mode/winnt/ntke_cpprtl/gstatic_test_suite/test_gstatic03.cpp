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
    TEST_LOOP = 301
  , TEST_CTOR
  , TEST_DTOR
  , MAGIC_VALUE
  };

  int res = 0;
  int const loop_check = TEST_LOOP * MAGIC_VALUE ;

}


namespace
{
  struct ctest
  {
    int value;

    ctest()
      : value ( MAGIC_VALUE )
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
    static ctest t301;
    res += t301.value;
    return t301.value;
  }


  int test_loop(int const& tl)
  {
    int ret = 0;
    for ( int i = 0; i < tl; ++i )
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


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include <rtl_framework_specific_header.h>


namespace
{
  enum
  {
    TEST_CTOR = 5467,
    TEST_DTOR,
  };


  int res = 0;
}


namespace
{

  class ctest
  {
  public:

    ctest()
    {
  #ifdef NT_KERNEL_MODE
      DbgPrint("test_gstatic02 ---> ctest::ctest()\n");
  #endif
      res += TEST_CTOR;
    }

    ~ctest()
    {
  #ifdef NT_KERNEL_MODE
      DbgPrint("test_gstatic02 ---> ctest::~ctest()\n");
  #endif
    }
  };

  ctest         t001;
}
ctest           t002;
static ctest    t003;


namespace cpprtl { namespace test { namespace gstatic
{

  int test_gstatic02()
  {
    return res - 3*TEST_CTOR ;
  }

}  }  }


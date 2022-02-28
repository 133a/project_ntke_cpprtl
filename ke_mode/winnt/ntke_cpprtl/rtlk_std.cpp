//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtl_framework.h"
#include "std_incl/exception"
#include "eh/eh_exception_code.h"


namespace std
{
  void terminate()
  {
    KeBugCheckEx
    (
      cpprtl::eh::EXCEPTION_CODE_CPP
    , cpprtl::eh::EXCEPTION_SUBCODE_STD_TERMINATE
    , 0
    , 0
    , 0
    );
  }
}

/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtl_framework_specific_header.h"
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


// icl15 vector copying support
void __cdecl terminate()
{
  std::terminate();
}


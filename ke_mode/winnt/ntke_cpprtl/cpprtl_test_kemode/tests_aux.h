/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TESTS_AUX_H_
#define TESTS_AUX_H_


namespace cpprtl_tests
{

  enum
  {
    RET_SUCCESS = 0
  , RET_ERROR_UNEXPECTED    = -1001
  , RET_ERROR_BAD_ALLOC     = -1002
  };


  typedef void (*testFT)(int&);  // tests interface


}  // namespace cpprtl_tests


#endif  // include guard


//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef TESTS_AUX_H_
#define TESTS_AUX_H_


namespace cpprtl_tests
{

  enum
  {
    RET_SUCCESS = 0
  , RET_ERROR_UNEXPECTED    = -1001
  , RET_ERROR_BAD_ALLOC     = -1002
  , RET_ERROR_TEST_FAILED   = -1003
  , RET_ERROR_GSTATIC       = -1004
  , RET_ERROR_EH_RUNONCE    = -1005
  };


  typedef void (*test_type)(int&);  // tests interface


  class test_payload
  {
    int        res;
    test_type  test;

  public:
    test_payload()
      : res  ( RET_ERROR_UNEXPECTED )
      , test ( 0 )
    {}

    test_payload(test_type t)
      : res  ( RET_ERROR_UNEXPECTED )
      , test ( t )
    {}

    void operator()()
    {
      test(res);
    }

    int result() const
    {
      return res;
    }
  };

}  // namespace cpprtl_tests


#endif  // include guard


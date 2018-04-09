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


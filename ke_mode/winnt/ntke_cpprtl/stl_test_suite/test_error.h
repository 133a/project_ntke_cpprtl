/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TEST_ERROR_H_
#define TEST_ERROR_H_


#include <exception>


namespace aux_
{

  struct test_error
    : public std::exception
  {
    int err;

    test_error(int const& e)
      : err ( e )
    {}
  };

}


#endif  // include guard


//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


/////////////////////////////////////////////
////
////  testing std::string
////  MT-safe
/////////////////////////////////////////////


#include <string>
#include <stdexcept>
#include <new>
#include "test_aux.h"


namespace cpprtl { namespace stl { namespace test
{
  namespace
  {
    enum
    {
      UNEXPECTED_ERROR           = 601
    , UNEXPECTED_STD_EXCEPTION   = 602
    , UNEXPECTED_STD_BAD_ALLOC   = 603
    , ERROR_OVERSIZED_STRING1    = 604
    , ERROR_OVERSIZED_STRING2    = 605
    , ERROR_OUT_OF_RANGE_FAILED  = 606
    };
  }


  bool test_06()
  {
    int state = 0;
    try
    {
      do
      {
        try
        {
          std::string oversized_str(std::string::size_type(-1), 'X');
          throw test_error(ERROR_OVERSIZED_STRING1);
        }
        catch (std::length_error&)
        {
          break;  // test is passed;
        }
        throw test_error(UNEXPECTED_ERROR);
      }
      while ( false );

      {
        std::wstring empty_str;
        do
        {
          try
          {
            std::wstring::value_type ch = empty_str.at(empty_str.size());
            throw test_error(ERROR_OUT_OF_RANGE_FAILED);
          }
          catch (std::out_of_range&)
          {
            break;  // test is passed;
          }
          throw test_error(UNEXPECTED_ERROR);
        }
        while ( false );
        do
        {
          try
          {
            empty_str.reserve(empty_str.max_size() + 1);
            throw test_error(ERROR_OVERSIZED_STRING2);
          }
          catch (std::length_error&)
          {
            break;  // test is passed;
          }
          throw test_error(UNEXPECTED_ERROR);
        }
        while ( false );
      }
    }
    catch (test_error const& t)
    {
      state = t.err;
    }
    catch (std::bad_alloc&)
    {
      state = UNEXPECTED_STD_BAD_ALLOC;
    }
    catch (std::exception&)
    {
      state = UNEXPECTED_STD_EXCEPTION;
    }
    catch (...)
    {
      state = UNEXPECTED_ERROR;
    }
    return 0 == state;
  }

}}}  // namespace cpprtl::stl::test

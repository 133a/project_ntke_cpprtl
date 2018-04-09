/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing std::string
////  MT-safe
/////////////////////////////////////////////


#include <string>
#include <stdexcept>
#include <new>

#include "test_error.h"


namespace
{
  enum
  {
    UNEXPECTED_ERROR           = -1
  , UNEXPECTED_STD_EXCEPTION   = -2
  , UNEXPECTED_STD_BAD_ALLOC   = -3
  , ERROR_OVERSIZED_STRING1    = -4
  , ERROR_OVERSIZED_STRING2    = -5
  , ERROR_OUT_OF_RANGE_FAILED  = -6
  };
}


namespace cpprtl { namespace test { namespace stl
{

  int test_stl06()
  {
    int state = 0;
    try
    {
      do
      {
        try
        {
          std::string oversized_str(std::string::size_type(-1), 'X');
          throw aux_::test_error(ERROR_OVERSIZED_STRING1);
        }
        catch (std::length_error&)
        {
          break;  // test is passed;
        }
        throw aux_::test_error(UNEXPECTED_ERROR);
      }
      while ( false );

      {
        std::wstring empty_str;
        do
        {
          try
          {
            std::wstring::value_type ch = empty_str.at(empty_str.size());
            throw aux_::test_error(ERROR_OUT_OF_RANGE_FAILED);
          }
          catch (std::out_of_range&)
          {
            break;  // test is passed;
          }
          throw aux_::test_error(UNEXPECTED_ERROR);
        }
        while ( false );
        do
        {
          try
          {
            empty_str.reserve(empty_str.max_size() + 1);
            throw aux_::test_error(ERROR_OVERSIZED_STRING2);
          }
          catch (std::length_error&)
          {
            break;  // test is passed;
          }
          throw aux_::test_error(UNEXPECTED_ERROR);
        }
        while ( false );
      }

    }
    catch (aux_::test_error const& t)
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

    return state;
  }

}  }  }


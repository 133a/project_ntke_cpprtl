//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


/////////////////////////////////////////////
////
////  testing std::vector<>
////  MT-safe
/////////////////////////////////////////////


#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <new>
#include "test_aux.h"
#include <check_irql.h>


namespace cpprtl { namespace stl { namespace test
{
  namespace
  {
    enum
    {
      UNEXPECTED_ERROR             = 201
    , UNEXPECTED_STD_EXCEPTION     = 202
    , UNEXPECTED_STD_BAD_ALLOC     = 203
    , TEST_ERROR_BAD_BALANCE       = 204
    , TEST_ERROR_UNEXPECTED_SIZE0  = 205
    , TEST_ERROR_UNEXPECTED_SIZE1  = 206
    , TEST_ERROR_UNEXPECTED_SIZE2  = 207
    , TEST_ERROR_UNEXPECTED_SIZE3  = 208
    , TEST_ERROR_UNEXPECTED_SIZE4  = 209
    , TEST_ERROR_UNEXPECTED_SIZE5  = 210
    , TEST_ERROR_WRONG_MIN         = 211
    , TEST_ERROR_WRONG_MAX         = 212
    , TEST_ERROR_FIND1             = 213
    , TEST_ERROR_FIND2             = 214
    , TEST_ERROR_WRONG_SUM         = 215
    , TEST_EMPTY_VECTOR_FAILED0    = 216
    , TEST_VECTOR_NOT_EQUAL        = 217
    , TEST_OUT_OF_RANGE_FAILED     = 218

    , TEST_VECTOR_SIZE             = 512
    , TEST_VECTOR_MIN_ELEMENT      = 0
    , TEST_VECTOR_MAX_ELEMENT      = TEST_VECTOR_SIZE - 1
    , TEST_VECTOR_SUM              = (TEST_VECTOR_MAX_ELEMENT + TEST_VECTOR_MIN_ELEMENT) * TEST_VECTOR_SIZE >> 1
    };

    typedef test_object<int>                                  int_obj;
    typedef std::vector<int_obj>                              int_obj_vector;
    typedef std::vector<int_obj, int_obj_nonpaged_allocator>  int_obj_nonpaged_vector;
    typedef std::vector<int_obj, int_obj_paged_allocator>     int_obj_paged_vector;
  }  // namespace


  bool test_02()
  {
    int state = 0;
    try
    {
      context ctx(0);
      {
        // the empty vector
        int_obj_vector test_vec1;
        if
        (
          !test_vec1.empty() ||
          test_vec1.size() != 0 ||
          std::distance(test_vec1.begin(), test_vec1.end()) != 0 ||
          std::distance(test_vec1.rbegin(), test_vec1.rend()) != 0
        )
        {
          throw test_error(TEST_EMPTY_VECTOR_FAILED0);
        }
        do
        {
          try
          {
            int_obj_vector::value_type v = test_vec1.at(0);
            throw test_error(TEST_OUT_OF_RANGE_FAILED);
          }
          catch (std::out_of_range&)
          {
            break;  // test is passed;
          }
          throw test_error(UNEXPECTED_ERROR);
        }
        while ( false );

        // default np-allocator
        for ( int i = TEST_VECTOR_MIN_ELEMENT ; i < TEST_VECTOR_SIZE ; ++i )
        {
          if ( !(i&1) )
          {
            test_vec1.push_back(int_obj(ctx, i));
          }
          else
          {
            test_vec1.insert(test_vec1.begin(), int_obj(ctx, i));
          }
        }
        if ( TEST_VECTOR_SIZE != test_vec1.size() )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE1);
        }
        if ( (*std::min_element(test_vec1.begin(), test_vec1.end())).value() != TEST_VECTOR_MIN_ELEMENT )
        {
          throw test_error(TEST_ERROR_WRONG_MIN);
        }
        if ( (*std::max_element(test_vec1.rbegin(), test_vec1.rend())).value() != TEST_VECTOR_MAX_ELEMENT )
        {
          throw test_error(TEST_ERROR_WRONG_MAX);
        }

        std::sort(test_vec1.begin(), test_vec1.end(), std::greater<typename int_obj_vector::value_type>());
        int_obj_vector::const_iterator iter = std::find(test_vec1.begin(), test_vec1.end(), int_obj(ctx, TEST_VECTOR_MAX_ELEMENT));
        if ( test_vec1.end() == iter || test_vec1.begin() != iter )
        {
          throw test_error(TEST_ERROR_FIND1);
        }

        std::reverse(test_vec1.begin(), test_vec1.end());
        int_obj_vector::reverse_iterator r_iter = std::find(test_vec1.rbegin(), test_vec1.rend(), int_obj(ctx, TEST_VECTOR_MIN_ELEMENT));
        if ( test_vec1.rend() == r_iter || test_vec1.rend()-1 != r_iter )
        {
          throw test_error(TEST_ERROR_FIND2);
        }

        int_obj_vector test_vec2;
        std::swap(test_vec1, test_vec2);
        if ( TEST_VECTOR_SIZE != test_vec2.size() || 0 != test_vec1.size() )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE2);
        }
        test_vec2.clear();
        if ( !test_vec2.empty() || 0 != std::distance(test_vec2.begin(), test_vec2.end()) )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE4);
        }


        // explicit np-allocator
        int_obj_nonpaged_vector test_vec3(TEST_VECTOR_SIZE, int_obj(ctx, -1));
        if ( TEST_VECTOR_SIZE != test_vec3.size() )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE3);
        }
        std::for_each(test_vec3.begin(), test_vec3.end(), test_object_init<typename int_obj_nonpaged_vector::value_type>());
        std::random_shuffle(test_vec3.begin(), test_vec3.end());
        if
        (
          TEST_VECTOR_SUM
        != 
          std::for_each
          (
            test_vec3.begin()
          , test_vec3.end()
          , test_object_sum<typename int_obj_nonpaged_vector::value_type>()
          ).get()
        )
        {
          throw test_error(TEST_ERROR_WRONG_SUM);
        }
        int_obj_nonpaged_vector test_vec4(test_vec3);
        if ( test_vec4 != test_vec3 )
        {
          throw test_error(TEST_VECTOR_NOT_EQUAL);
        }

        // explicit p-allocator
        if ( IRQL_IS(==PASSIVE_LEVEL) )
        {
          int_obj_paged_vector test_vec5;
          std::copy(test_vec4.begin(), test_vec4.end(), std::back_inserter(test_vec5));
          if ( test_vec5.size() != test_vec4.size() )
          {
            throw test_error(TEST_ERROR_UNEXPECTED_SIZE5);
          }
        }
      }

      if ( !ctx.ok() )
      {
        throw test_error(TEST_ERROR_BAD_BALANCE);
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

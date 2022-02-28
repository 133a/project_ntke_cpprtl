//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


/////////////////////////////////////////////
////
////  testing std::map<>
////  MT-safe
/////////////////////////////////////////////


#include <map>
#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
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
      UNEXPECTED_ERROR             = 401
    , UNEXPECTED_STD_EXCEPTION     = 402
    , UNEXPECTED_STD_BAD_ALLOC     = 403
    , TEST_ERROR_BAD_BALANCE       = 404
    , TEST_ERROR_UNEXPECTED_SIZE0  = 405
    , TEST_ERROR_UNEXPECTED_SIZE1  = 406
    , TEST_ERROR_UNEXPECTED_SIZE2  = 407
    , TEST_ERROR_UNEXPECTED_SIZE3  = 408
    , TEST_ERROR_UNEXPECTED_SIZE4  = 409
    , TEST_ERROR_UNEXPECTED_SIZE5  = 410
    , TEST_ERROR_WRONG_MIN         = 411
    , TEST_ERROR_WRONG_MAX         = 412
    , TEST_ERROR_FIND1             = 413
    , TEST_ERROR_FIND2             = 414
    , TEST_ERROR_FIND3             = 415
    , TEST_ERROR_FIND4             = 416
    , TEST_ERROR_WRONG_SUM         = 417
    , TEST_EMPTY_MAP_FAILED0       = 418
    , TEST_MAP_NOT_EQUAL           = 419

    , TEST_MAP_SIZE                = 512
    , TEST_MAP_MIN_ELEMENT         = 0
    , TEST_MAP_MAX_ELEMENT         = TEST_MAP_SIZE - 1
    , TEST_MAP_SUM                 = (TEST_MAP_MAX_ELEMENT + TEST_MAP_MIN_ELEMENT) * TEST_MAP_SIZE >> 1
    };

    typedef test_object<int>        int_obj;
    typedef std::map<int, int_obj>  int_obj_map;

    typedef std::map
    <
      int
    , int_obj
    , std::less<int>
    , int_obj_pair_nonpaged_allocator
    >
    int_obj_nonpaged_map;

    typedef std::map
    <
      int
    , int_obj
    , std::greater<int>
    , int_obj_pair_nonpaged_allocator
    >
    int_obj_nonpaged_greater_map;

    typedef std::map
    <
      int
    , int_obj
    , std::less<int>
    , int_obj_pair_paged_allocator
    >
    int_obj_paged_map;

    template <typename T>
    struct test_object_sum_for_map
      : protected test_object_sum<T>
    {
      using typename test_object_sum<T>::value_type;

      template <typename K>
      void operator()(std::pair<K, T> const& p)
      {
        sum += p.second.value();
      }

      value_type get() const
      {
        return sum;
      }
    };
  }  // namespace


  bool test_04()
  {
    int state = 0;
    try
    {
      context ctx(0);
      {
        // an empty map
        int_obj_map test_map1;
        if
        (
          !test_map1.empty() ||
          test_map1.size() != 0 ||
          std::distance(test_map1.begin(), test_map1.end()) != 0 ||
          std::distance(test_map1.rbegin(), test_map1.rend()) != 0
        )
        {
          throw test_error(TEST_EMPTY_MAP_FAILED0);
        }

        for ( int i = TEST_MAP_MIN_ELEMENT ; i <= TEST_MAP_MAX_ELEMENT ; ++i )
        {
          test_map1.insert(int_obj_map::value_type(i, int_obj(ctx, i)));
        }
        if ( TEST_MAP_SIZE != test_map1.size() )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE1);
        }
        if ( (*std::min_element(test_map1.begin(), test_map1.end())).second.value() != TEST_MAP_MIN_ELEMENT )
        {
          throw test_error(TEST_ERROR_WRONG_MIN);
        }
        if ( (*std::max_element(test_map1.rbegin(), test_map1.rend())).second.value() != TEST_MAP_MAX_ELEMENT )
        {
          throw test_error(TEST_ERROR_WRONG_MAX);
        }

        int_obj_map::const_iterator iter = test_map1.find(TEST_MAP_MAX_ELEMENT);
        if ( test_map1.end() == iter || --test_map1.end() != iter || (*iter).second.value() != TEST_MAP_MAX_ELEMENT )
        {
          throw test_error(TEST_ERROR_FIND1);
        }
        iter = test_map1.find(TEST_MAP_MIN_ELEMENT);
        if ( test_map1.end() == iter || test_map1.begin() != iter || (*iter).second.value() != TEST_MAP_MIN_ELEMENT )
        {
          throw test_error(TEST_ERROR_FIND2);
        }

        int_obj_map test_map2(test_map1);
        if ( TEST_MAP_SIZE != test_map2.size() )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE2);
        }
        if ( test_map1 != test_map2 )
        {
          throw test_error(TEST_MAP_NOT_EQUAL);
        }
        test_map1.clear();
        std::swap(test_map1, test_map2);
        if ( !test_map2.empty() || 0 != std::distance(test_map2.begin(), test_map2.end()) )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE4);
        }


        // explicit np-allocator
        int_obj_nonpaged_map test_map3;
        for ( int i = TEST_MAP_MIN_ELEMENT ; i <= TEST_MAP_MAX_ELEMENT ; ++i )
        {
          test_map3.insert(std::make_pair(i, int_obj(ctx, i)));
        }
        if ( TEST_MAP_SIZE != test_map3.size() )
        {
          throw test_error(TEST_ERROR_UNEXPECTED_SIZE3);
        }
        if
        (
          TEST_MAP_SUM
        !=
          std::for_each
          (
            test_map3.begin()
          , test_map3.end()
          , test_object_sum_for_map<typename int_obj_nonpaged_map::mapped_type>()
          ).get()
        )
        {
          throw test_error(TEST_ERROR_WRONG_SUM);
        }

        int_obj_nonpaged_greater_map test_map4(test_map3.begin(), test_map3.end());
        int_obj_nonpaged_greater_map::const_iterator gr_iter = test_map4.find(TEST_MAP_MAX_ELEMENT);
        if ( test_map4.end() == gr_iter || test_map4.begin() != gr_iter || (*gr_iter).second.value() != TEST_MAP_MAX_ELEMENT )
        {
          throw test_error(TEST_ERROR_FIND3);
        }
        gr_iter = test_map4.find(TEST_MAP_MIN_ELEMENT);
        if ( test_map4.end() == gr_iter || --test_map4.end() != gr_iter || (*gr_iter).second.value() != TEST_MAP_MIN_ELEMENT )
        {
          throw test_error(TEST_ERROR_FIND4);
        }


        // explicit paged-allocator
        if ( IRQL_IS(==PASSIVE_LEVEL) )
        {
          int_obj_paged_map test_map5;
          std::copy(test_map4.begin(), test_map4.end(), std::inserter(test_map5, test_map5.begin()));
          if ( test_map5.size() != test_map4.size() )
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

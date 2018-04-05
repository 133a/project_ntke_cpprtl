/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing std::list<>
////  MT-safe
/////////////////////////////////////////////


#include <list>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <new>

#include "test_context.h"
#include "test_error.h"
#include "kalloc.h"
#include <check_irql.h>


namespace
{
  enum
  {
    UNEXPECTED_ERROR             = -1
  , UNEXPECTED_STD_EXCEPTION     = -2
  , UNEXPECTED_STD_BAD_ALLOC     = -3
  , TEST_ERROR_BAD_BALANCE       = -4
  , TEST_ERROR_UNEXPECTED_SIZE0  = -5
  , TEST_ERROR_UNEXPECTED_SIZE1  = -6
  , TEST_ERROR_UNEXPECTED_SIZE2  = -7
  , TEST_ERROR_UNEXPECTED_SIZE3  = -8
  , TEST_ERROR_UNEXPECTED_SIZE4  = -9
  , TEST_ERROR_UNEXPECTED_SIZE5  = -10
  , TEST_ERROR_WRONG_MIN         = -11
  , TEST_ERROR_WRONG_MAX         = -12
  , TEST_ERROR_FIND1             = -13
  , TEST_ERROR_FIND2             = -14
  , TEST_ERROR_WRONG_SUM         = -15
  , TEST_EMPTY_LIST_FAILED0      = -16
  , TEST_LIST_NOT_EQUAL          = -17

  , TEST_LIST_SIZE               = 512
  , TEST_LIST_MIN_ELEMENT        = 0
  , TEST_LIST_MAX_ELEMENT        = TEST_LIST_SIZE - 1
  , TEST_LIST_SUM                = (TEST_LIST_MAX_ELEMENT + TEST_LIST_MIN_ELEMENT) * TEST_LIST_SIZE / 2
  };


  typedef aux_::test_object<int>                                int_obj;
  typedef std::list<int_obj>                                    int_obj_list;  // the default stl allocator is supposed to use the nonpaged memory
  typedef std::list<int_obj, aux_::int_obj_nonpaged_allocator>  int_obj_nonpaged_list;
  typedef std::list<int_obj, aux_::int_obj_paged_allocator>     int_obj_paged_list;
}


namespace cpprtl { namespace test { namespace stl
{

  int test_stl03()
  {
    int state = 0;

    try  //  the main test scope
    {
      aux_::test_context ctx(0);
      ctx.state = 0;
      {

      ////  testing an empty list
        int_obj_list test_lst1;
        if
        (
          ! test_lst1.empty()
          || test_lst1.size() != 0
          || std::distance(test_lst1.begin(), test_lst1.end()) != 0
          || std::distance(test_lst1.rbegin(), test_lst1.rend()) != 0
        )
        {
          throw aux_::test_error(TEST_EMPTY_LIST_FAILED0);
        }

      ////  testing a list with the default np-allocator
        for ( int i = TEST_LIST_MIN_ELEMENT ; i < TEST_LIST_SIZE ; ++i )
        {
          if ( i % 2 )
          {
            test_lst1.push_back(int_obj(ctx, i));
          }
          else
          {
            test_lst1.push_front(int_obj(ctx, i));
          }
        }
        if ( TEST_LIST_SIZE != test_lst1.size() )
        {
          throw aux_::test_error(TEST_ERROR_UNEXPECTED_SIZE1);
        }
        if ( (*std::min_element(test_lst1.begin(), test_lst1.end())).value() != TEST_LIST_MIN_ELEMENT )
        {
          throw aux_::test_error(TEST_ERROR_WRONG_MIN);
        }
        if ( (*std::max_element(test_lst1.rbegin(), test_lst1.rend())).value() != TEST_LIST_MAX_ELEMENT )
        {
          throw aux_::test_error(TEST_ERROR_WRONG_MAX);
        }

        test_lst1.sort(std::greater<typename int_obj_list::value_type>());
        int_obj_list::const_iterator iter = std::find(test_lst1.begin(), test_lst1.end(), int_obj(ctx, TEST_LIST_MAX_ELEMENT));
        if ( test_lst1.end() == iter || test_lst1.begin() != iter )
        {
          throw aux_::test_error(TEST_ERROR_FIND1);
        }

        std::reverse(test_lst1.begin(), test_lst1.end());
        int_obj_list::reverse_iterator r_iter = std::find(test_lst1.rbegin(), test_lst1.rend(), int_obj(ctx, TEST_LIST_MIN_ELEMENT));
        if ( test_lst1.rend() == r_iter || --test_lst1.rend() != r_iter )
        {
          throw aux_::test_error(TEST_ERROR_FIND2);
        }

        int_obj_list test_lst2;
        std::swap(test_lst1, test_lst2);
        if ( TEST_LIST_SIZE != test_lst2.size() || 0 != test_lst1.size() )
        {
          throw aux_::test_error(TEST_ERROR_UNEXPECTED_SIZE2);
        }
        test_lst2.clear();
        if ( !test_lst2.empty() || 0 != std::distance(test_lst2.begin(), test_lst2.end()) )
        {
          throw aux_::test_error(TEST_ERROR_UNEXPECTED_SIZE4);
        }


      ////  a list with the explicit np-allocator
        int_obj_nonpaged_list test_lst3(TEST_LIST_SIZE, int_obj(ctx, -1));
        if ( TEST_LIST_SIZE != test_lst3.size() )
        {
          throw aux_::test_error(TEST_ERROR_UNEXPECTED_SIZE3);
        }
        std::for_each(test_lst3.begin(), test_lst3.end(), aux_::test_object_init<typename int_obj_nonpaged_list::value_type>());
        if
        (
          TEST_LIST_SUM !=
          std::for_each
          (
            test_lst3.begin()
          , test_lst3.end()
          , aux_::test_object_sum<typename int_obj_nonpaged_list::value_type>()
          ).get()
        )
        {
          throw aux_::test_error(TEST_ERROR_WRONG_SUM);
        }
        int_obj_nonpaged_list test_lst4(test_lst3);
        if ( test_lst4 != test_lst3 )
        {
          throw aux_::test_error(TEST_LIST_NOT_EQUAL);
        }


      ////  a list with the explicit paged-allocator
        if ( IRQL_IS(==PASSIVE_LEVEL) )
        {
          int_obj_paged_list test_lst5;
          std::copy(test_lst4.begin(), test_lst4.end(), std::back_inserter(test_lst5));
          if ( test_lst5.size() != test_lst4.size() )
          {
            throw aux_::test_error(TEST_ERROR_UNEXPECTED_SIZE5);
          }
        }
      }

      if ( 0 != ctx.balance() )
      {
        throw aux_::test_error(TEST_ERROR_BAD_BALANCE);
      }
    }
    catch ( aux_::test_error const& t )
    {
      state = t.err;
    }
    catch ( std::bad_alloc& )
    {
      state = UNEXPECTED_STD_BAD_ALLOC;
    }
    catch ( std::exception& )
    {
      state = UNEXPECTED_STD_EXCEPTION;
    }
    catch ( ... )
    {
      state = UNEXPECTED_ERROR;
    }

    return state;
  }

}  }  }


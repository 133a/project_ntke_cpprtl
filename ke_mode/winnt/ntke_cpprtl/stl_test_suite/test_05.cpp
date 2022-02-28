//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


/////////////////////////////////////////////
////
////  testing std::auto_ptr<>
////  MT-safe
/////////////////////////////////////////////


#include <memory>
#include <stdexcept>
#include <new>
#include "test_aux.h"


namespace cpprtl { namespace stl { namespace test
{
  namespace
  {
    enum
    {
      UNEXPECTED_ERROR          = 501
    , UNEXPECTED_STD_EXCEPTION  = 502
    , UNEXPECTED_STD_BAD_ALLOC  = 503
    , TEST_ERROR_BAD_BALANCE    = 504
    , TEST_ERROR_NONEMPTY_PTR   = 505
    , TEST_ERROR_EMPTY_PTR      = 506
    , TEST_ERROR_WRONG_OWNER    = 507
    , TEST_ERROR_WRONG_VALUE    = 508

    , INTOBJ_DEFAULT_VALUE      = 555
    };

    typedef  test_object<int>        int_obj;
    typedef  std::auto_ptr<int_obj>  int_obj_ptr;
  }  // namespace


  bool test_05()
  {
    int state = 0;
    try
    {
      context ctx(0);
      {
        int_obj_ptr au1;
        if
        (
          au1.get() ||
          au1.operator->()
        )
        {
          throw test_error(TEST_ERROR_NONEMPTY_PTR);
        }

        au1.reset(new int_obj(ctx, INTOBJ_DEFAULT_VALUE * 2));
        if
        (
          !au1.get() ||
          !au1.operator->()
        )
        {
          throw test_error(TEST_ERROR_EMPTY_PTR);
        }
        au1.reset(new int_obj(ctx, INTOBJ_DEFAULT_VALUE));
        int_obj_ptr au2 = au1;
        if
        (
          au1.get() ||
          au1.operator->() ||
          !au2.get() ||
          !au2.operator->()
        )
        {
          throw test_error(TEST_ERROR_WRONG_OWNER);
        }
        if
        (
          INTOBJ_DEFAULT_VALUE != au2->value() ||
          INTOBJ_DEFAULT_VALUE != (*au2).value()
        )
        {
          throw test_error(TEST_ERROR_WRONG_VALUE);
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

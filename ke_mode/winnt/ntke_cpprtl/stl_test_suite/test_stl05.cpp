/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing std::auto_ptr<>
////  MT-safe
/////////////////////////////////////////////


#include <memory>
#include <stdexcept>
#include <new>

#include "test_context.h"
#include "test_error.h"


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


  typedef  aux_::test_object<int>  int_obj;
  typedef  std::auto_ptr<int_obj>  int_obj_ptr;
}


namespace cpprtl { namespace test { namespace stl
{

  int test_stl05()
  {
    int state = 0;
    try
    {
      aux_::test_context ctx(0);
      ctx.state = 0;
      {
        int_obj_ptr au1;
        if
        (
          au1.get()
        ||
          au1.operator->()
        )
        {
          throw aux_::test_error(TEST_ERROR_NONEMPTY_PTR);
        }

        au1.reset(new int_obj(ctx, INTOBJ_DEFAULT_VALUE * 2));
        if
        (
          !au1.get()
        ||
          !au1.operator->()
        )
        {
          throw aux_::test_error(TEST_ERROR_EMPTY_PTR);
        }
        int_obj* tmp = new int_obj(ctx, INTOBJ_DEFAULT_VALUE);
        au1.reset(tmp);
        int_obj_ptr au2 = au1;
        if
        (
          au1.get()
        ||
          au1.operator->()
        ||
          !au2.get()
        ||
          !au2.operator->()
        )
        {
          throw aux_::test_error(TEST_ERROR_WRONG_OWNER);
        }
        if
        (
          INTOBJ_DEFAULT_VALUE != au2->value()
        ||
          INTOBJ_DEFAULT_VALUE != (*au2).value()
        )
        {
          throw aux_::test_error(TEST_ERROR_WRONG_VALUE);
        }
      }

      if ( 0 != ctx.balance() )
      {
        throw aux_::test_error(TEST_ERROR_BAD_BALANCE);
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


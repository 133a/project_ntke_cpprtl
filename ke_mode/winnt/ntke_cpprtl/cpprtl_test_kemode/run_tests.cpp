//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "run_tests.h"
#include "tests_aux.h"
#include "test_irql.h"
#include "test_workitem.h"
#include "test_thread.h"
#include "test_dpc.h"
#include "test_apc.h"
// RTL test stuff
#include <eh/eh_test_suite/eh_test_suite.h>
#include <rtti/rtti_test_suite/rtti_test_loop.h>
#include <gstatic_test_suite/gstatic_test_loop.h>
#include <stl_test_suite/stl_test_loop.h>


namespace cpprtl_tests
{
  namespace
  {
    enum
    {
    #ifdef _M_ARM
      CYCLE_COUNT           = 32
    #else
      CYCLE_COUNT           = 128
    #endif
    , CYCLE_COUNT_WORKITEM  = CYCLE_COUNT
    , CYCLE_COUNT_DPC       = CYCLE_COUNT  // memento DPC_WATCHDOG_VIOLATION (DISPATCH_LEVEL >=nt6.2)
    , CYCLE_COUNT_APC       = CYCLE_COUNT
    , CYCLE_COUNT_THREAD    = CYCLE_COUNT
    , FACTOR_EH_MT          = 8
    , FACTOR_EH_ST          = 32
    , FACTOR_RTTI           = 64
    , FACTOR_STL            = 1
    };

    // cycling wrapper for eh thread-safe tests
    template <unsigned CYCLE_CNT>
    void test_eh_thread_safe(int& res)
    {
      res = RET_SUCCESS;
    #ifdef TEST_EH
      for ( unsigned i = 0 ; i < CYCLE_CNT && RET_SUCCESS == res ; ++i )
      {
        res = cpprtl::eh::test::run_thread_safe();
      }
    #endif
    }

    // cycling wrapper for eh thread-unsafe tests
    template <unsigned CYCLE_CNT>
    void test_eh_thread_unsafe(int& res)
    {
      res = RET_SUCCESS;
    #ifdef TEST_EH
      for ( unsigned i = 0 ; i < CYCLE_CNT && RET_SUCCESS == res ; ++i )
      {
        res = cpprtl::eh::test::run_thread_unsafe();
      }
    #endif
    }

    // cycling wrapper for rtti thread-safe tests
    template <unsigned CYCLE_CNT>
    void test_rtti_thread_safe(int& res)
    {
      res = RET_SUCCESS;
    #ifdef TEST_RTTI
      for ( unsigned i = 0 ; i < CYCLE_CNT && RET_SUCCESS == res ; ++i )
      {
        res = cpprtl::rtti::test::run_thread_safe();
      }
    #endif
    }

    // cycling wrapper for stl tests
    template <unsigned CYCLE_CNT>
    void test_stl(int& res)
    {
      res = RET_SUCCESS;
    #ifdef TEST_STL
      for ( unsigned i = 0 ; i < CYCLE_CNT && RET_SUCCESS == res ; ++i )
      {
        res = cpprtl::stl::test::run();
      }
    #endif
    }


    test_type const irql_tests[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT * FACTOR_EH_MT>
    , test_eh_thread_unsafe  <CYCLE_COUNT * FACTOR_EH_ST>
    , test_rtti_thread_safe  <CYCLE_COUNT * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT * FACTOR_STL>
    , 0  // keep last
    };

    test_type const workitem_tests[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_WORKITEM * FACTOR_EH_MT>
    , test_eh_thread_unsafe  <CYCLE_COUNT_WORKITEM * FACTOR_EH_ST>
    , test_rtti_thread_safe  <CYCLE_COUNT_WORKITEM * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_WORKITEM * FACTOR_STL>
    , 0  // keep last
    };

    test_type const workitem_tests_mt[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_WORKITEM * FACTOR_EH_MT>
    , test_rtti_thread_safe  <CYCLE_COUNT_WORKITEM * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_WORKITEM * FACTOR_STL>
    , 0  // keep last
    };

    test_type const dpc_tests[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_DPC * FACTOR_EH_MT>
    , test_eh_thread_unsafe  <CYCLE_COUNT_DPC * FACTOR_EH_ST>
    , test_rtti_thread_safe  <CYCLE_COUNT_DPC * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_DPC * FACTOR_STL>
    , 0  // keep last
    };

    test_type const dpc_tests_mt[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_DPC * FACTOR_EH_MT>
    , test_rtti_thread_safe  <CYCLE_COUNT_DPC * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_DPC * FACTOR_STL>
    , 0  // keep last
    };

    test_type const apc_tests[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_APC * FACTOR_EH_MT>
    , test_eh_thread_unsafe  <CYCLE_COUNT_APC * FACTOR_EH_ST>
    , test_rtti_thread_safe  <CYCLE_COUNT_APC * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_APC * FACTOR_STL>
    , 0  // keep last
    };

    test_type const apc_tests_mt[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_APC * FACTOR_EH_MT>
    , test_rtti_thread_safe  <CYCLE_COUNT_APC * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_APC * FACTOR_STL>
    , 0  // keep last
    };

    test_type const thread_tests[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_THREAD * FACTOR_EH_MT>
    , test_eh_thread_unsafe  <CYCLE_COUNT_THREAD * FACTOR_EH_ST>
    , test_rtti_thread_safe  <CYCLE_COUNT_THREAD * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_THREAD * FACTOR_STL>
    , 0  // keep last
    };

    test_type const thread_tests_mt[] =
    {
      test_eh_thread_safe    <CYCLE_COUNT_THREAD * FACTOR_EH_MT>
    , test_rtti_thread_safe  <CYCLE_COUNT_THREAD * FACTOR_RTTI>
    , test_stl               <CYCLE_COUNT_THREAD * FACTOR_STL>
    , 0  // keep last
    };
  }  // namespace


  int run()
  {
  // check static/global ctoring
  #ifdef TEST_GSTATIC
    {
      if ( RET_SUCCESS != cpprtl::gstatic::test::run_once() )
      {
        return RET_ERROR_GSTATIC;
      }
    }
  #endif  // TEST_GSTATIC

  // run in the main thread at various IRQLs
  #ifdef TEST_IRQL
    {
      int res = test_irql(irql_tests);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_IRQL

  #ifdef TEST_EH
    {
      if ( RET_SUCCESS != cpprtl::eh::test::run_once() )
      {
        return RET_ERROR_EH_RUNONCE;
      }
    }
  #endif  // TEST_EH

  // run in a queued work item
  #ifdef TEST_WORKITEM
    {
      int res = test_workitem(workitem_tests);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_workitem_mt(workitem_tests_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_WORKITEM

  // run in DPC
  #ifdef TEST_DPC
    {
      int res = test_dpc(dpc_tests);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_dpc_mt(dpc_tests_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_DPC

  // run in kAPC
  #ifdef TEST_APC
    {
      int res = test_apc(apc_tests);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_apc_mt(apc_tests_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_APC

  // run in kernel threads  
  #ifdef TEST_THREAD
    {
      int res = test_thread(thread_tests);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_thread_mt(thread_tests_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_THREAD

    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests

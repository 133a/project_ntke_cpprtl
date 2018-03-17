/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "run_tests.h"
#include "tests_aux.h"
#include "test_irql.h"
#include "test_workitem.h"
#include "test_thread.h"
#include "test_dpc.h"
#include "test_apc.h"

//  RTL testing stuff
#include <eh/eh_test_suite/eh_test_suite.h>
#include <rtti/rtti_test_suite/rtti_test_loop.h>
#include <gstatic_test_suite/gstatic_test_loop.h>
#include <stl_test_suite/stl_test_loop.h>


namespace
{
  enum
  {
    CYCLE_COUNT          = 128          // memento DPC_WATCHDOG_VIOLATION at >=nt6.2 when running at DISPATCH_LEVEL.
  , CYCLE_COUNT_WORKITEM = CYCLE_COUNT
  , CYCLE_COUNT_DPC      = CYCLE_COUNT
  , CYCLE_COUNT_APC      = CYCLE_COUNT
  , CYCLE_COUNT_THREAD   = CYCLE_COUNT
  , FACTOR_EH_MT         = 8
  , FACTOR_EH_ST         = 128
  , FACTOR_RTTI          = 64
  , FACTOR_STL           = 1
  };
}


namespace
{

  // wrapper for static ctoring/dtoring tests, not for run in threads, moreover invoke it only once 'cos the right result depends on a static state of the test lib
  void test_gstatic(int& res)
  {
    res = cpprtl_tests::RET_SUCCESS;
  #ifdef TEST_GSTATIC
    res = cpprtl::test::gstatic::run();
  #endif
  }


  // cycling wrapper for eh thread-safe tests
  template <unsigned CYCLE_CNT>
  void test_eh_thread_safe(int& res)
  {
    res = cpprtl_tests::RET_SUCCESS;
  #ifdef TEST_EH
    for (unsigned i = 0 ; i < CYCLE_CNT && cpprtl_tests::RET_SUCCESS == res ; ++i)
    {
      res = cpprtl::test::eh::run_thread_safe();
    }
  #endif
  }


  // cycling wrapper for eh thread-unsafe tests
  template <unsigned CYCLE_CNT>
  void test_eh_thread_unsafe(int& res)
  {
    res = cpprtl_tests::RET_SUCCESS;
  #ifdef TEST_EH
    for (unsigned i = 0 ; i < CYCLE_CNT && cpprtl_tests::RET_SUCCESS == res ; ++i)
    {
      res = cpprtl::test::eh::run_thread_unsafe();
    }
  #endif
  }


  // cycling wrapper for rtti thread-safe tests
  template <unsigned CYCLE_CNT>
  void test_rtti_thread_safe(int& res)
  {
    res = cpprtl_tests::RET_SUCCESS;
  #ifdef TEST_RTTI
    for (unsigned i = 0 ; i < CYCLE_CNT && cpprtl_tests::RET_SUCCESS == res ; ++i)
    {
      res = cpprtl::test::rtti::run_thread_safe();
    }
  #endif
  }


  // cycling wrapper for stl tests
  template <unsigned CYCLE_CNT>
  void test_stl(int& res)
  {
    res = cpprtl_tests::RET_SUCCESS;
  #ifdef TEST_STL
    for (unsigned i = 0 ; i < CYCLE_CNT && cpprtl_tests::RET_SUCCESS == res ; ++i)
    {
      res = cpprtl::test::stl::run();
    }
  #endif
  }

}  // namespace


namespace
{

  cpprtl_tests::testFT test_funcs_irql[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT * FACTOR_EH_MT>
  , test_eh_thread_unsafe  <CYCLE_COUNT * FACTOR_EH_ST>
  , test_rtti_thread_safe  <CYCLE_COUNT * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_workitem[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_WORKITEM * FACTOR_EH_MT>
  , test_eh_thread_unsafe  <CYCLE_COUNT_WORKITEM * FACTOR_EH_ST>
  , test_rtti_thread_safe  <CYCLE_COUNT_WORKITEM * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_WORKITEM * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_workitem_mt[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_WORKITEM * FACTOR_EH_MT>
  , test_rtti_thread_safe  <CYCLE_COUNT_WORKITEM * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_WORKITEM * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_dpc[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_DPC * FACTOR_EH_MT>
  , test_eh_thread_unsafe  <CYCLE_COUNT_DPC * FACTOR_EH_ST>
  , test_rtti_thread_safe  <CYCLE_COUNT_DPC * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_DPC * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_dpc_mt[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_DPC * FACTOR_EH_MT>
  , test_rtti_thread_safe  <CYCLE_COUNT_DPC * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_DPC * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_apc[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_APC * FACTOR_EH_MT>
  , test_eh_thread_unsafe  <CYCLE_COUNT_APC * FACTOR_EH_ST>
  , test_rtti_thread_safe  <CYCLE_COUNT_APC * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_APC * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_apc_mt[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_APC * FACTOR_EH_MT>
  , test_rtti_thread_safe  <CYCLE_COUNT_APC * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_APC * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_thread[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_THREAD * FACTOR_EH_MT>
  , test_eh_thread_unsafe  <CYCLE_COUNT_THREAD * FACTOR_EH_ST>
  , test_rtti_thread_safe  <CYCLE_COUNT_THREAD * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_THREAD * FACTOR_STL>
  , 0  //  keep last
  };


  cpprtl_tests::testFT test_funcs_thread_mt[] =
  {
    test_eh_thread_safe    <CYCLE_COUNT_THREAD * FACTOR_EH_MT>
  , test_rtti_thread_safe  <CYCLE_COUNT_THREAD * FACTOR_RTTI>
  , test_stl               <CYCLE_COUNT_THREAD * FACTOR_STL>
  , 0  //  keep last
  };

}  // namespace



namespace cpprtl_tests
{

  int run()
  {

  //// check static/global ctoring
    {
      int res = RET_ERROR_UNEXPECTED;
      test_gstatic(res);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }


  //// run in the main thread at various IRQLs
    {
      int res = test_irql(test_funcs_irql);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }


  //// run in a queued work item
  #ifdef TEST_WORKITEM
    {
      int res = test_workitem(test_funcs_workitem);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_workitem_mt(test_funcs_workitem_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_WORKITEM


  //// run in a DPC
  #ifdef TEST_DPC
    {
      int res = test_dpc(test_funcs_dpc);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_dpc_mt(test_funcs_dpc_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_DPC


  //// run in a kAPC
  #ifdef TEST_APC
    {
      int res = test_apc(test_funcs_apc);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_apc_mt(test_funcs_apc_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_APC


  //// run in kernel threads  
  #ifdef TEST_THREADS
    {
      int res = test_thread(test_funcs_thread);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
      res = test_thread_mt(test_funcs_thread_mt);
      if ( RET_SUCCESS != res )
      {
        return res;
      }
    }
  #endif  // TEST_THREADS


    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


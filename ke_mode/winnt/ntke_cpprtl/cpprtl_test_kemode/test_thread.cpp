/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <new>
#include "tests_aux.h"
#include "test_thread.h"
#include "thread_type.h"


#ifndef TEST_GSTATIC  //  if global/static init-ors are not available
namespace  
{
  class thread_bunch
  {
    static int                ret_values [cpprtl_tests::THR_NUM];
    static aux_::thread_type* threads    [cpprtl_tests::THR_NUM];  //  using 'thread_t*' instead of just 'thread_t' 'cos if the current cpprtl lacks the static values ctoring/dtoring support

    thread_bunch();  //  static class

  public:
    static void start(cpprtl_tests::testFT func)
    {
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        ret_values[i] = cpprtl_tests::RET_ERROR_THREAD_NOT_STARTED;
      }
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        if ( ! threads[i] )
        {
          threads[i] = new thread_type();  // the default new is 'NonPagedPool'
        }
        if 
        (
        #ifdef NTKE_KTHREAD
          ! NT_SUCCESS ( threads[i]->spawn(typeaux::functee(func, typeaux::make_ref(ret_values[i]))) )
        #else
          ! NT_SUCCESS ( threads[i]->spawn(cpprtl_tests::testFT_adapter(func, ret_values[i])) )
        #endif
        )
        {
          throw cpprtl_tests::test_error(cpprtl_tests::RET_ERROR_THREAD_SPAWN);
        }
      }
    }

    static void clear()
    {
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        delete threads[i];
        threads[i] = 0;
      }
    }

    static void acquire()
    {
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        if ( threads[i] )
        {
          threads[i]->acquire(STATUS_SUCCESS);
          threads[i]->detach();
        }
      }
    }

    static int const& ret_value(int const& idx)
    {
      return ret_values[idx];
    }
  };
  int                thread_bunch::ret_values [cpprtl_tests::THR_NUM];
  aux_::thread_type* thread_bunch::threads    [cpprtl_tests::THR_NUM] = { 0 };
}

#else  // TEST_GSTATIC - if ntke_cpprtl with global/static init-ors is used

namespace
{
  class thread_bunch
  {
    static int                ret_values [cpprtl_tests::THR_NUM];
    static aux_::thread_type  threads    [cpprtl_tests::THR_NUM];

    thread_bunch();  //  static class

  public:
    static void start(cpprtl_tests::testFT func)
    {
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        ret_values[i] = cpprtl_tests::RET_ERROR_THREAD_NOT_STARTED;
      }
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        if 
        (
        #ifdef NTKE_KTHREAD
          ! NT_SUCCESS ( threads[i].spawn(typeaux::functee(func, typeaux::make_ref(ret_values[i]))) )
        #else
          ! NT_SUCCESS ( threads[i].spawn(cpprtl_tests::testFT_adapter(func, ret_values[i])) )
        #endif
        )
        {
          throw cpprtl_tests::test_error(cpprtl_tests::RET_ERROR_THREAD_SPAWN);
        }
      }
    }

    static void clear()
    {
      return;
    }

    static void acquire()
    {
      for (int i = 0; i < cpprtl_tests::THR_NUM; ++i)
      {
        threads[i].acquire(STATUS_SUCCESS);
        threads[i].detach();
      }
    }

    static int const& ret_value(int const& idx)
    {
      return ret_values[idx];
    }
  };
  int               thread_bunch::ret_values [cpprtl_tests::THR_NUM];
  aux_::thread_type thread_bunch::threads    [cpprtl_tests::THR_NUM];
}
#endif  // TEST_GSTATIC


namespace
{
  struct thread_bunch_auto_release
  {
    ~thread_bunch_auto_release()
    {
      thread_bunch::clear();
    }
  };

  struct thread_bunch_auto_acquire
  {
    ~thread_bunch_auto_acquire()
    {
      thread_bunch::acquire();
    }
  };
}


namespace cpprtl_tests
{

  int test_thread(testFT test_funcs[])
  {
    DbgPrint("spawning kthreads %d\n", THR_NUM);
    try
    {
      thread_bunch_auto_release threads_release;
      for ( int i = 0 ; test_funcs[i] ; ++i )
      {
        {
          thread_bunch_auto_acquire threads_acquire;
          thread_bunch::start(test_funcs[i]);
        }
        for ( int j = 0 ; j < THR_NUM ; ++j )
        {
          if ( RET_SUCCESS != thread_bunch::ret_value(j) )
          {
            DbgPrint("cpprtl_tests::test_thread() : test_func %d ret %d from thread &d\n", i, thread_bunch::ret_value(j), j);
            return thread_bunch::ret_value(j);
          }
        }
      }
    }
    catch ( test_error const& te )
    {
      return te.get();
    }
    catch ( std::bad_alloc& )  //  if 'new thread_t()' is failed
    {
      return RET_ERROR_BAD_ALLOC;
    }
    catch ( ... )
    {
      return RET_ERROR_UNEXPECTED;
    }
    return RET_SUCCESS;
  }

}  //  namespace cpprtl_tests


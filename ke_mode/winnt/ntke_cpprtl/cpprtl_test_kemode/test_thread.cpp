/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <cstddef>
#include <new>
#include "thread_type.h"
#include "aux_cpu.h"
#include "aux_scoped_ptr.h"
#include "aux_task.h"
#include "tests_aux.h"
#include "test_thread.h"


namespace cpprtl_tests
{
  enum
  {
    THREAD_FACTOR = 8
  };


  enum
  {
    RET_ERROR_THREAD_ALLOC         = -1401
  , RET_ERROR_THREAD_SPAWN         = -1402
  , RET_ERROR_THREAD_TASK_SPAWN    = -1403
  };


  class thread_adapter
  {
    int&    res;
    testFT  ftest;

  public:
    thread_adapter(testFT f, int& i)
      : res   ( i )
      , ftest ( f )
    {}

    void operator()() const
    {
      ftest(res);
    }
  };


  class thread_task
    : public aux_::task_bunch<aux_::thread_type>
  {
    typedef aux_::task_bunch<aux_::thread_type> base_type;

  private:
    aux_::scoped_array_ptr<int> task_ret;

  private:
    void operator[](std::size_t) {}

  public:
    thread_task()
      : task_ret ( 0 )
    {}

    ~thread_task()
    {
      acquire();
    }

    bool spawn(std::size_t const& n, testFT f)
    {
      task_num = n;
      spawned = 0;
      task.reset(new(std::nothrow) task_type[task_num]);
      task_ret.reset(new(std::nothrow) int[task_num]);
      if ( !task_ret.get() || !task_ret.get() )
      {
        status = RET_ERROR_THREAD_ALLOC;
        return false;
      }
      for ( spawned = 0 ; spawned < task_num ; ++spawned )
      {
        task_ret[spawned] = RET_ERROR_UNEXPECTED;
        if 
        (
        #ifdef NTKE_KTHREAD
          ! NT_SUCCESS ( task[spawned].spawn(typeaux::functee(f, typeaux::make_ref(task_ret[spawned]))) )
        #else
          ! NT_SUCCESS ( task[spawned].spawn(thread_adapter(f, task_ret[spawned])) )
        #endif
        )
        {
          status = RET_ERROR_THREAD_SPAWN;
          return false;
        }
      }
      status = RET_SUCCESS;
      return true;
    }

    void acquire()
    {
      while ( spawned > 0 )
      {
        --spawned;
        task[spawned].acquire(STATUS_SUCCESS);
        task[spawned].detach();
      }
    }

    int const& result(std::size_t const& idx) const
    {
      return task_ret[idx];
    }

  };


  int test_thread_impl(testFT tests[], std::size_t const& task_num)
  {
    for ( unsigned i = 0 ; tests[i] ; ++i )
    {
      DbgPrint("test_thread_impl() : spawning threads[%u] for test[%u]\n", unsigned(task_num), i);
      thread_task task;
      if ( !task.spawn(task_num, tests[i]) )
      {
        return RET_ERROR_THREAD_TASK_SPAWN;
      }
      task.acquire();

      for ( unsigned k = 0 ; k < task_num ; ++k )
      {
        if ( RET_SUCCESS != task.result(k) )
        {
          DbgPrint("test_thread_impl() ERROR : test[%u]=%i at thread[%u]\n", i, task.result(k), k);
          return task.result(k);
        }
      }
    }
    DbgPrint("test_thread_impl() : 0\n");
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

  int test_thread(testFT tests[])
  {
    DbgPrint("test_thread()\n");
    return test_thread_impl(tests, 1);
  }


  int test_thread_mt(testFT tests[])
  {
    std::size_t const task_num = aux_::get_number_processors() * THREAD_FACTOR;
    DbgPrint("test_thread_mt() : task_num=%u\n", unsigned(task_num));
    if ( task_num > 1 )
    {
      return test_thread_impl(tests, task_num);
    }
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


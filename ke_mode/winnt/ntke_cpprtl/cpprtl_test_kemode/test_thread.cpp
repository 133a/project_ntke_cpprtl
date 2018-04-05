/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <cstddef>
#include <new>
#include "aux_cpu.h"
#include "aux_task.h"
#include "tests_aux.h"
#include "test_thread.h"
#include "thread_type.h"


namespace cpprtl_tests
{
  enum
  {
    THREAD_FACTOR = 8
  };


  enum
  {
    RET_ERROR_THREAD_TASK_SPAWN  = -1401
  };


  template <typename PAYLOAD>
  class kthread
  {
    typedef PAYLOAD payload_type;

    template <typename PAYLOAD>
    class payload_transfer
    {
      typedef PAYLOAD payload_type;
      payload_type& payload;

    public:
      explicit payload_transfer(payload_type& pld)
        : payload ( pld )
      {}

      void operator()()
      {
        return payload();
      }
    };

  private:
    bool          spawned;
    thread_type   thread;
    payload_type  payload;

  public:
    kthread()
      : spawned ( false )
    {}

    ~kthread()
    {
      acquire();
    }

    template <typename PLD>
    bool spawn(PLD const& pld, size_t const& = 0)
    {
      bool result = false;
      if ( !spawned )
      {
        payload = pld;
        result = spawned = NT_SUCCESS(thread.spawn(payload_transfer<payload_type>(payload)));
      }
      return result;
    }

    void acquire()
    {
      thread.acquire(STATUS_SUCCESS);
      thread.detach();
      spawned = false;
    }

    payload_type const& get_payload() const
    {
      return payload;
    }
  };

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

  int test_thread_impl(testFT tests[], std::size_t const& task_num)
  {
    typedef aux_::task_bunch<kthread<test_payload> > thread_task;
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
        int const result = task[k].get_payload().result();
        if ( RET_SUCCESS != result )
        {
          DbgPrint("test_thread_impl() ERROR : test[%u]=%i at thread[%u]\n", i, result, k);
          return result;
        }
      }
    }
    DbgPrint("test_thread_impl() : 0\n");
    return RET_SUCCESS;
  }


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


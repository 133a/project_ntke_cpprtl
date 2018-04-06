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
#include "aux_workitem.h"
#include "sync_type.h"
#include "tests_aux.h"
#include "test_workitem.h"


namespace cpprtl_tests
{
  enum
  {
    WORKITEM_FACTOR = 2
  };


  enum
  {
    RET_ERROR_WORKITEM_TASK_SPAWN  = -1101
  };


  template <typename PAYLOAD>
  class work_item
    : public aux_::work_item
  {
    typedef PAYLOAD          payload_type;
    typedef mutex_type       lock_type;
    typedef auto_mutex_type  auto_lock_type;

  private:
    lock_type     lock;
    event_type    evt;
    bool          spawned;
    payload_type  pld;

  public:
    work_item()
      : evt      ( false, event_type::MANUAL_RESET )
      , spawned  ( false )
    {}

    virtual ~work_item()
    {
      auto_lock_type lck(lock);
    }

    template <typename PLD>
    bool spawn(PLD const& p, std::size_t const& = 0)
    {
      auto_lock_type lck(lock);
      if ( !spawned )
      {
        pld = p;
        evt.reset();
        enqueue();
        return spawned = true;
      }
      return false;
    }

    void acquire()
    {
      evt.acquire(STATUS_SUCCESS);
    }

    payload_type const& get_payload() const
    {
      return pld;
    }

  protected:
    virtual void payload()
    {
      DbgPrint("work_item::payload() %p\n", KeGetCurrentThread());
      pld();
      {
        auto_lock_type lck(lock);
        spawned = false;
        evt.set();
      }
    }

  };

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

  int test_workitem_impl(testFT tests[], std::size_t const& task_num)
  {
    typedef aux_::task_bunch<work_item<test_payload> > work_item_task;
    for ( unsigned i = 0 ; tests[i] ; ++i )
    {
      DbgPrint("test_workitem_impl() : spawning work_items[%u] for test[%u]\n", unsigned(task_num), i);
      work_item_task task;
      if ( !task.spawn(task_num, tests[i]) )
      {
        return RET_ERROR_WORKITEM_TASK_SPAWN;
      }
      task.acquire();

      for ( unsigned k = 0 ; k < task_num ; ++k )
      {
        int const result = task[k].get_payload().result();
        if ( RET_SUCCESS != result )
        {
          DbgPrint("test_workitem_impl() ERROR : test[%u]=%i at work_item[%u]\n", i, result, k);
          return result;
        }
      }
    }
    DbgPrint("test_workitem_impl() : 0\n");
    return RET_SUCCESS;
  }


  int test_workitem(testFT tests[])
  {
    DbgPrint("test_workitem()\n");
    return test_workitem_impl(tests, 1);
  }


  int test_workitem_mt(testFT tests[])
  {
    std::size_t const task_num = aux_::get_number_processors() * WORKITEM_FACTOR;
    DbgPrint("test_workitem_mt() : task_num=%u\n", unsigned(task_num));
    if ( task_num > 1 )
    {
      return test_workitem_impl(tests, task_num);
    }
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


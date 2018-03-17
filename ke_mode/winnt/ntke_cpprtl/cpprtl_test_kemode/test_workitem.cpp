/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <cstddef>
#include <new>
#include "aux_cpu.h"
#include "aux_sync.h"
#include "aux_task.h"
#include "aux_workitem.h"
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
    RET_ERROR_WORKITEM_ALLOC         = -1101
  , RET_ERROR_WORKITEM_SPAWN         = -1102
  , RET_ERROR_WORKITEM_TASK_SPAWN    = -1103
  };


  class work_item
    : public aux_::work_item
  {
    typedef aux_::spin_lock       lock_type;
    typedef aux_::auto_spin_lock  auto_lock_type;

  private:
    lock_type     lock;
    aux_::kevent  evt;
    int           status;
    testFT        ftest;

  public:
    work_item()
      : evt     ( false, aux_::kevent::MANUAL_RESET )
      , status  ( RET_ERROR_UNEXPECTED )
      , ftest   ( 0 )
    {}

    virtual ~work_item()
    {
      auto_lock_type lck(lock);
    }

    BOOLEAN spawn(testFT f)
    {
      bool do_enqueue = false;
      {
        auto_lock_type lck(lock);
        if ( 0 == ftest )
        {
          evt.reset();
          status = RET_ERROR_UNEXPECTED;
          ftest = f;
          do_enqueue = true;
        }
      }
      if ( do_enqueue )
      {
        enqueue();
        return TRUE;
      }
      return FALSE;
    }

    NTSTATUS acquire()
    {
      return evt.acquire(STATUS_SUCCESS);
    }

    int result() const
    {
      return status;
    }

  protected:
    virtual void payload()
    {
      DbgPrint("work_item::payload() %p\n", KeGetCurrentThread());
      ftest(status);
      {
        auto_lock_type lck(lock);
        ftest = 0;
        evt.set();
      }
    }

  };


  class work_item_task
    : public aux_::task_bunch<work_item>
  {
    typedef aux_::task_bunch<work_item> base_type;

  public:
    work_item_task()
    {}

    ~work_item_task()
    {
      acquire();
    }

    bool spawn(std::size_t const& n, testFT f)
    {
      task_num = n;
      spawned = 0;
      task.reset(new(std::nothrow) task_type[task_num]);
      if ( !task.get() )
      {
        status = RET_ERROR_WORKITEM_ALLOC;
        return false;
      }
      for ( spawned = 0 ; spawned < task_num ; ++spawned )
      {
        if ( !task[spawned].spawn(f) )
        {
          status = RET_ERROR_WORKITEM_SPAWN;
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
        task[--spawned].acquire();
      }
    }
  };


  int test_workitem_impl(testFT tests[], std::size_t const& task_num)
  {
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
        if ( RET_SUCCESS != task[k].result() )
        {
          DbgPrint("test_workitem_impl() ERROR : test[%u]=%i at work_item[%u]\n", i, task[k].result(), k);
          return task[k].result();
        }
      }
    }
    DbgPrint("test_workitem_impl() : 0\n");
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

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


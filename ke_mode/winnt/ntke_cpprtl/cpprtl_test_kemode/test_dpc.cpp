/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <cstddef>
#include <new>
#include "tests_aux.h"
#include "aux_dpc.h"
#include "aux_cpu.h"
#include "aux_irql.h"
#include "aux_sync.h"
#include "aux_task.h"
#include "test_dpc.h"


namespace cpprtl_tests
{
  enum
  {
    DPC_FACTOR = 1
  };

  enum
  {
    RET_ERROR_DPC_ALLOC         = -1201
  , RET_ERROR_DPC_SPAWN         = -1202
  , RET_ERROR_DPC_TASK_SPAWN    = -1203
  };


  class kdpc
    : public aux_::kdpc
  {
    typedef aux_::spin_lock           lock_type;
    typedef aux_::auto_spin_lock      auto_lock_type;
    typedef aux_::dpc_auto_spin_lock  dpc_auto_lock_type;

  private:       
    lock_type     lock;
    aux_::kevent  evt;
    int           res;
    testFT        ftest;

  public:
    kdpc()
      : evt    ( false, aux_::kevent::MANUAL_RESET )
      , res    ( RET_ERROR_UNEXPECTED )
      , ftest  ( 0 )
    {}

    virtual ~kdpc()
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
          res = RET_ERROR_UNEXPECTED;
          ftest = f;
          do_enqueue = true;
        }
      }
      if ( do_enqueue )
      {
        return enqueue(0, 0);
      }
      return FALSE;
    }

    NTSTATUS acquire()
    {
      return evt.acquire(STATUS_SUCCESS);
    }

    int result() const
    {
      return res;
    }

  protected:
    virtual void payload(void*, void*)
    {
      DbgPrint("kdpc::payload() %u\n", unsigned(aux_::get_current_processor()));
      ftest(res);
      {
        dpc_auto_lock_type lck(lock);
        ftest = 0;
        evt.set();
      }
    }

  };


  class dpc_task
    : public aux_::task_bunch<kdpc>
  {
    typedef aux_::task_bunch<kdpc> base_type;

  public:
    dpc_task()
    {}

    ~dpc_task()
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
        status = RET_ERROR_DPC_ALLOC;
      }

      {
        ULONG const cpu_num = aux_::get_number_processors();
        aux_::auto_irql_raiser irql(DISPATCH_LEVEL);
        for ( spawned = 0 ; spawned < task_num ; ++spawned )
        {
          task[spawned].set_processor(static_cast<CHAR>(spawned % cpu_num));
          if ( !task[spawned].spawn(f) )
          {
            status = RET_ERROR_DPC_SPAWN;
            return false;
          }
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


  int test_dpc_impl(testFT tests[], std::size_t const& task_num)
  {
    for ( unsigned i = 0 ; tests[i] ; ++i )
    {
      DbgPrint("test_dpc_impl() : spawning kdpcs[%u] for test[%u]\n", unsigned(task_num), i);
      dpc_task task;
      if ( !task.spawn(task_num, tests[i]) )
      {
        return RET_ERROR_DPC_TASK_SPAWN;
      }
      task.acquire();

      for ( unsigned k = 0 ; k < task_num ; ++k )
      {
        if ( RET_SUCCESS != task[k].result() )
        {
          DbgPrint("test_dpc_impl() ERROR : test[%u]=%i at kdpc[%u]\n", i, task[k].result(), k);
          return task[k].result();
        }
      }
    }
    DbgPrint("test_dpc_impl() : 0\n");
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests



namespace cpprtl_tests
{
  int test_dpc(testFT tests[])
  {
    DbgPrint("test_dpc()\n");
    return test_dpc_impl(tests, 1);
  }


  int test_dpc_mt(testFT tests[])
  {
    std::size_t const task_num = aux_::get_number_processors() * DPC_FACTOR;
    DbgPrint("test_dpc_mt() : task_num=%u\n", unsigned(task_num));
    if ( task_num > 1 )
    {
      return test_dpc_impl(tests, task_num);
    }
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


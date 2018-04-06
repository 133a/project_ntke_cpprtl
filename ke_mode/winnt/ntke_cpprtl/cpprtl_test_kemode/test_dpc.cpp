/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <cstddef>
#include <new>
#include "aux_cpu.h"
#include "aux_dpc.h"
#include "aux_irql.h"
#include "aux_task.h"
#include "sync_type.h"
#include "tests_aux.h"
#include "test_dpc.h"


namespace cpprtl_tests
{
  enum
  {
    DPC_FACTOR = 1
  };

  enum
  {
    RET_ERROR_DPC_TASK_SPAWN  = -1201
  };


  template <typename PAYLOAD>
  class kdpc
    : public aux_::kdpc
  {
    typedef PAYLOAD                  payload_type;
    typedef spin_lock_type           lock_type;
    typedef auto_spin_lock_type      auto_lock_type;
    typedef dpc_auto_spin_lock_type  dpc_auto_lock_type;

    enum { DONT_SET_PROCESSOR = -1 };

  private:       
    lock_type     lock;
    event_type    evt;
    bool          spawned;
    payload_type  pld;

  public:
    kdpc()
      : evt     ( false, event_type::MANUAL_RESET )
      , spawned ( false )
    {}

    virtual ~kdpc()
    {
      auto_lock_type lck(lock);
    }

    template <typename PLD>
    bool spawn(PLD const& p, std::size_t const& idx = DONT_SET_PROCESSOR)
    {
      auto_lock_type lck(lock);
      if ( !spawned )
      {
        pld = p;
        evt.reset();
        if ( idx != DONT_SET_PROCESSOR )
        {
          set_processor(static_cast<CHAR>(idx % aux_::get_number_processors()));
        }
        return spawned = enqueue(0, 0) == TRUE;
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
    virtual void payload(void*, void*)
    {
      DbgPrint("kdpc::payload() %u\n", unsigned(aux_::get_current_processor()));
      pld();
      {
        dpc_auto_lock_type lck(lock);
        spawned = false;
        evt.set();
      }
    }

  };

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

  int test_dpc_impl(testFT tests[], std::size_t const& task_num)
  {
    typedef aux_::task_bunch<kdpc<test_payload> > dpc_task;
    for ( unsigned i = 0 ; tests[i] ; ++i )
    {
      DbgPrint("test_dpc_impl() : spawning kdpcs[%u] for test[%u]\n", unsigned(task_num), i);
      dpc_task task;
      {
        aux_::auto_irql_raiser irql(DISPATCH_LEVEL);
        if ( !task.spawn(task_num, tests[i]) )
        {
          return RET_ERROR_DPC_TASK_SPAWN;
        }
      }
      task.acquire();

      for ( unsigned k = 0 ; k < task_num ; ++k )
      {
        int const result = task[k].get_payload().result();
        if ( RET_SUCCESS != result )
        {
          DbgPrint("test_dpc_impl() ERROR : test[%u]=%i at kdpc[%u]\n", i, result, k);
          return result;
        }
      }
    }
    DbgPrint("test_dpc_impl() : 0\n");
    return RET_SUCCESS;
  }


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


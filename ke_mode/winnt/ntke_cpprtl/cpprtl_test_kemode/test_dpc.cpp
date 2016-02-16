/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <new>
#include "tests_aux.h"
#include "aux_dpc.h"
#include "aux_sync.h"
#include "test_dpc.h"
#include "scoped_ptr.h"


namespace
{
  ULONG get_number_processors()
  {
  #if NTDDI_VERSION < 0x05010000  // nt5.1
    return *KeNumberProcessors;
  #else
    return KeNumberProcessors;
  #endif
  }


  ULONG get_current_processor()
  {
  #if NTDDI_VERSION < 0x06010000  // nt6.1
    ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);  // recommended
    return KeGetCurrentProcessorNumber();
  #else
    return KeGetCurrentProcessorNumberEx(0);  // any IRQL
  #endif
  }


  class kdpc
    : public aux_::kdpc
  {
    typedef aux_::spin_lock           lock_type;
    typedef aux_::auto_spin_lock      auto_lock_type;
    typedef aux_::dpc_auto_spin_lock  dpc_auto_lock_type;

  public:
    typedef cpprtl_tests::testFT testFT;

  private:       
    lock_type     lock;
    aux_::kevent  evt;
    int           res;
    testFT        ftest;

  public:
    kdpc()
      : evt    ( false, aux_::kevent::MANUAL_RESET )
      , res    ( cpprtl_tests::RET_ERROR_UNEXPECTED )
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
        if ( lck.locked() && 0 == ftest )
        {
          evt.reset();
          res = cpprtl_tests::RET_ERROR_UNEXPECTED;
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
      DbgPrint("kdpc::payload() %d\n", get_current_processor());
      if ( ftest )
      {
        ftest(res);
      }
      dpc_auto_lock_type lck(lock);
      if ( lck.locked() )
      {
        ftest = 0;
        evt.set();
      }
    }

  };

}


namespace cpprtl_tests
{

  //  run in a DPC
  int test_dpc(testFT test_funcs[])
  {
    kdpc dpc;
    for ( int i = 0 ; test_funcs[i] ; ++i )
    {
      if ( ! dpc.spawn(test_funcs[i]) )
      {
        return RET_ERROR_DPC_SPAWN;
      }
      if ( !NT_SUCCESS(dpc.acquire()) )
      {
        return RET_ERROR_DPC_ACQUIRE;
      }
      if ( RET_SUCCESS != dpc.result() )
      {
        DbgPrint("cpprtl_tests::test_dpc() : test_func %d ret %d\n", i, dpc.result());
        return dpc.result();
      }
    }
    return RET_SUCCESS;
  }

}  //  namespace cpprtl_tests


namespace
{

  class dpc_task
  {
    aux_::scoped_array_ptr<kdpc> dpc;
    char const dpc_num;
    int* const dpc_status;

  public:
    dpc_task(char const& proc_n, int* const st)
      : dpc        ( new(std::nothrow) kdpc [proc_n])  // NPaged no-throwing new[]
      , dpc_num    ( proc_n )
      , dpc_status ( st )
    {}

    bool spawn(cpprtl_tests::testFT f)
    {
      for ( char idx = 0 ; idx < dpc_num ; ++idx )
      {
        if ( ! dpc.get() )
        {
          dpc_status[idx] = cpprtl_tests::RET_ERROR_DPC_TASK_ALLOC;
          return false;
        }
        dpc[idx].set_processor(dpc_num - idx - 1);
        if ( ! dpc[idx].spawn(f) )
        {
          dpc_status[idx] = cpprtl_tests::RET_ERROR_DPC_SPAWN;
          return false;
        }
        dpc_status[idx] = STATUS_SUCCESS;
      }
      return true;
    }

    bool acquire()
    {
      bool ok = true;
      if ( dpc.get() )
      {
        for ( char idx = 0 ; idx < dpc_num && NT_SUCCESS(dpc_status[idx]) ; ++idx )
        {
          if ( !NT_SUCCESS(dpc[idx].acquire()) )
          {
            dpc_status[idx] = cpprtl_tests::RET_ERROR_DPC_ACQUIRE;
            ok = false;
            continue;
          }
          dpc_status[idx] = dpc[idx].result();
        }
      }
      return ok;
    }
  };


  class dpc_task_auto_acquire
  {
    dpc_task* dpc;

  public:
    explicit dpc_task_auto_acquire(dpc_task* const d)
      : dpc ( d )
    {}

    void release()
    {
      dpc = 0;
    }

    ~dpc_task_auto_acquire()
    {
      if ( dpc )
      {
        dpc->acquire();
      }
    }
  };

}


namespace cpprtl_tests
{

  int test_dpc_mt(testFT test_funcs[])
  {
    unsigned const knp = get_number_processors();
    if ( knp > 1 )
    {
      aux_::scoped_array_ptr<int> dpc_status (new(std::nothrow) int [knp]);
      if ( ! dpc_status.get() )
      {
        return RET_ERROR_DPC_STATUS_ALLOC;
      }
 
      dpc_task dpc(knp, dpc_status.get());
      for ( int i = 0 ; test_funcs[i] ; ++i )
      {
        dpc_task_auto_acquire acq(&dpc);  // spawn guard
        if ( ! dpc.spawn(test_funcs[i]) )
        {
          return RET_ERROR_DPC_TASK_SPAWN;
        }
        acq.release();
        if ( ! dpc.acquire() )
        {
          return RET_ERROR_DPC_TASK_ACQUIRE;
        }
  
        for ( unsigned k = 0 ; k < knp ; ++k )
        {
          if ( RET_SUCCESS != dpc_status[k] )
          {
            DbgPrint("cpprtl_tests::test_dpc_mt() : test_func %d ret %d\n", i, dpc_status[k]);
            return dpc_status[k];
          }
        }
      }
    }
    return RET_SUCCESS;
  }

}  //  namespace cpprtl_tests


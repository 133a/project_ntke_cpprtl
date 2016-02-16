/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <new>
#include "tests_aux.h"
#include "test_apc.h"
#include "aux_apc.h"
#include "aux_sync.h"
#include "scoped_ptr.h"
#include "thread_type.h"


namespace
{

  class kapc
    : public aux_::kapc
  {
    typedef aux_::fast_mutex           lock_type;
    typedef aux_::auto_fast_mutex      auto_lock_type;
    typedef aux_::apc_auto_fast_mutex  apc_auto_lock_type;

  public:
    typedef cpprtl_tests::testFT testFT;

  private:
    lock_type     lock;
    aux_::kevent  evt;
    int           res;
    testFT        ftest;

  public:
    kapc()
      : evt    ( false, aux_::kevent::MANUAL_RESET )
      , res    ( cpprtl_tests::RET_ERROR_UNEXPECTED )
      , ftest  ( 0 )
    {
    }

    virtual ~kapc()
    {
      auto_lock_type lck(lock);
    }

    template <typename APC_TYPE>
    BOOLEAN spawn(APC_TYPE const& apc_type, PKTHREAD kt, testFT f)
    {
      bool do_enqueue = false;
      {
        auto_lock_type lck(lock);
        if ( lck.locked() && 0 == ftest )
        {
          evt.reset();
          res = cpprtl_tests::RET_ERROR_APC_PENDING;
          ftest = f;
          do_enqueue = true;
        }
      }
      if ( do_enqueue )
      {
        return enqueue(apc_type, kt, 0, 0, 0);
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
    void payload_impl()
    {
      if ( ftest )
      {
        if ( cpprtl_tests::RET_SUCCESS == res || cpprtl_tests::RET_ERROR_APC_PENDING == res )
        {
          ftest(res);
        }
      }
    }

    virtual void special_kernel_payload(PKNORMAL_ROUTINE*, void**, void**, void**)
    {
      DbgPrint("kapc::special_kernel_payload() %d\n", KeGetCurrentThread());
      payload_impl();

      apc_auto_lock_type lck(lock);
      if ( lck.locked() )
      {
        ftest = 0;
        evt.set();
      }
    }

    virtual void regular_kernel_payload(PKNORMAL_ROUTINE*, void**, void**, void**)
    {
      DbgPrint("kapc::regular_kernel_payload() %d\n", KeGetCurrentThread());
      payload_impl();
    }

    virtual void regular_normal_payload(void*, void*, void*)
    {
      DbgPrint("kapc::regular_normal_payload() %d\n", KeGetCurrentThread());
      payload_impl();

      auto_lock_type lck(lock);
      if ( lck.locked() )
      {
        ftest = 0;
        evt.set();
      }
    }

    virtual void rundown_payload()
    {
      DbgPrint("kapc::rundown_payload()\n");

      auto_lock_type lck(lock);
      if ( lck.locked() )
      {
        ftest = 0;
        evt.set();
      }
    }

  };

}


namespace
{

  //  run in the kAPC
  void test_apc_impl(cpprtl_tests::testFT test_funcs[], PKTHREAD kt_rx, int& res)
  {
    aux_::scoped_ptr<kapc> apc(new(std::nothrow) kapc);  //  NPaged no-throwing new

    if ( ! apc.get() )
    {
      res = cpprtl_tests::RET_ERROR_APC_ALLOC;
      return;
    }

    for ( int i = 0 ; test_funcs[i] ; ++i )
    {

    // regular kAPC
      if ( ! apc->spawn(aux_::kapc::regular(), kt_rx, test_funcs[i]) )
      {
        res = cpprtl_tests::RET_ERROR_APC_SPAWN_REGULAR;
        return;
      }
      if ( ! NT_SUCCESS(apc->acquire()) )
      {
        res = cpprtl_tests::RET_ERROR_APC_ACQUIRE_REGULAR;
        return;
      }
      if ( cpprtl_tests::RET_SUCCESS != apc->result() )
      {
        DbgPrint("test_apc_impl() regular : test_func %d ret %d\n", i, apc->result());
        res = apc->result();
        return;
      }

    // special kAPC
      if ( ! apc->spawn(aux_::kapc::special(), kt_rx, test_funcs[i]) )
      {
        res = cpprtl_tests::RET_ERROR_APC_SPAWN_SPECIAL;
        return;
      }
      if ( ! NT_SUCCESS(apc->acquire()) )
      {
        res = cpprtl_tests::RET_ERROR_APC_ACQUIRE_SPECIAL;
        return;
      }
      if ( cpprtl_tests::RET_SUCCESS != apc->result() )
      {
        DbgPrint("test_apc_impl() special : test_func %d ret %d\n", i, apc->result());
        res = apc->result();
        return;
      }
    }

    res = cpprtl_tests::RET_SUCCESS;
  }


  class test_apc_adapter
  {
    int& res;
    cpprtl_tests::testFT* test_funcs;
    PKTHREAD kt_rx;

  public:
    test_apc_adapter(cpprtl_tests::testFT tf[], int& i, PKTHREAD kt)
      : res        ( i )
      , test_funcs ( tf )
      , kt_rx      ( kt )
    {}

    void operator()() const
    {
      test_apc_routine(test_funcs, res, kt_rx);
    }

    static void test_apc_routine(cpprtl_tests::testFT test_funcs[], int& res, PKTHREAD kt)
    {
      test_apc_impl(test_funcs, kt ? kt : KeGetCurrentThread(), res);
    }
  };

}


namespace
{

  class apc_rx_thread_adapter
  {
    aux_::kevent& evt;

  public:
    explicit apc_rx_thread_adapter(aux_::kevent& e)
      : evt ( e )
    {}

    void operator()() const
    {
      apc_rx_thread_routine(evt);
    }

    static void apc_rx_thread_routine(aux_::kevent& evt)
    {
      evt.acquire(STATUS_SUCCESS);
    }
  };

}


namespace
{
// a separate APC-receiving threads to be spawned to provide APC-callbacks with the most stack possible for the tests to complete (EH test08/21 are using a very stack consuming engine)


  // spawns a thread sending kAPCs to itself
  class sending_apc_to_thread_itself
  {
    aux_::thread_type kt;

  public:
    int start(cpprtl_tests::testFT test_funcs[], int& res)
    {
      if
      (
      #ifdef NTKE_KTHREAD
        ! NT_SUCCESS ( kt.spawn(typeaux::functee(&test_apc_adapter::test_apc_routine, test_funcs, typeaux::make_ref(res), static_cast<PKTHREAD>(0))) )
      #else
        ! NT_SUCCESS ( kt.spawn(test_apc_adapter(test_funcs, res, 0)) )
      #endif
      )
      {
        return cpprtl_tests::RET_ERROR_THREAD_SPAWN;
      }
      return cpprtl_tests::RET_SUCCESS;
    }

    ~sending_apc_to_thread_itself()
    {
      kt.acquire(STATUS_SUCCESS);
    }
  };


  // spawns a separate APC-receiving thread
  class sending_apc_to_separate_thread
  {
    aux_::thread_type  kt_tx;
    aux_::thread_type  kt_rx;
    aux_::kevent       stop_evt;

  public:
    sending_apc_to_separate_thread() : stop_evt ( false, aux_::kevent::MANUAL_RESET) {}

    int start(cpprtl_tests::testFT test_funcs[], int& res)
    {
      if
      (
      #ifdef NTKE_KTHREAD
        ! NT_SUCCESS ( kt_rx.spawn(typeaux::functee(&apc_rx_thread_adapter::apc_rx_thread_routine, typeaux::make_ref(stop_evt))) )
      #else
        ! NT_SUCCESS ( kt_rx.spawn(apc_rx_thread_adapter(stop_evt)) )
      #endif
      )
      {
        return cpprtl_tests::RET_ERROR_THREAD_SPAWN;
      }
      if
      (
      #ifdef NTKE_KTHREAD
        ! NT_SUCCESS ( kt_tx.spawn(typeaux::functee(&test_apc_adapter::test_apc_routine, test_funcs, typeaux::make_ref(res), kt_rx.native_type())) )
      #else
        ! NT_SUCCESS ( kt_tx.spawn(test_apc_adapter(test_funcs, res, kt_rx.native_type())) )
      #endif                                               
      )
      {
        return cpprtl_tests::RET_ERROR_THREAD_SPAWN;
      }
      return cpprtl_tests::RET_SUCCESS;
    }

    ~sending_apc_to_separate_thread()
    {
      kt_tx.acquire(STATUS_SUCCESS);
      stop_evt.set();
      kt_rx.acquire(STATUS_SUCCESS);
    }
  };

}


namespace cpprtl_tests
{
  //  the only APC-receiving thread is spawned at a time to proceed with MT-unsafe tests
  int test_apc(testFT test_funcs[])
  {
    int status = RET_ERROR_UNEXPECTED;

    {
      sending_apc_to_thread_itself task;
      if ( task.start(test_funcs, status) != RET_SUCCESS )
      {
        return RET_ERROR_APC_TASK_SPAWN;
      }
    }
    if ( RET_SUCCESS != status )
    {
      return status;
    }

    {
      sending_apc_to_separate_thread task;
      if ( task.start(test_funcs, status) != RET_SUCCESS )
      {
        return RET_ERROR_APC_TASK_SPAWN;
      }
    }
    if ( RET_SUCCESS != status )
    {
      return status;
    }

    return status;
  }


  //  simultaneously spawned APC-receiving threads
  int test_apc_mt(testFT test_funcs[])
  {
    typedef sending_apc_to_separate_thread sending_apc;

    int status[APC_NUM] = { RET_ERROR_UNEXPECTED };
    {
      sending_apc tasks[APC_NUM];
      for ( int idx = 0 ; idx < APC_NUM ; ++idx )
      {
        if ( tasks[idx].start(test_funcs, status[idx]) != RET_SUCCESS )
        {
          return RET_ERROR_APC_TASK_SPAWN;
        }
      }
    }
    for ( int idx = 0 ; idx < APC_NUM ; ++idx )
    {
      if ( RET_SUCCESS != status[idx] )
      {
        return status[idx];
      }
    }
    return RET_SUCCESS;
  }

}  //  namespace cpprtl_tests


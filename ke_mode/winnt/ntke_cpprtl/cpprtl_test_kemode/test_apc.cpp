/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <new>
#include "tests_aux.h"
#include "test_apc.h"
#include "aux_apc.h"
#include "aux_cpu.h"
#include "aux_sync.h"
#include "aux_task.h"
#include "aux_scoped_ptr.h"
#include "thread_type.h"


namespace cpprtl_tests
{
  enum
  {
    APC_FACTOR = 2
  };

  enum
  {
    RET_ERROR_APC_ALLOC       = -1301
  , RET_ERROR_APC_SPAWN       = -1302
  , RET_ERROR_APC_ACQUIRE     = -1303
  , RET_ERROR_APC_PENDING     = -1304
  , RET_ERROR_APC_TASK_SPAWN  = -1305
  };


  class kapc
    : public aux_::kapc
  {
    typedef aux_::fast_mutex           lock_type;
    typedef aux_::auto_fast_mutex      auto_lock_type;
    typedef aux_::apc_auto_fast_mutex  apc_auto_lock_type;

  private:
    lock_type     lock;
    aux_::kevent  evt;
    int           res;
    testFT        ftest;

  public:
    kapc()
      : evt    ( false, aux_::kevent::MANUAL_RESET )
      , res    ( RET_ERROR_UNEXPECTED )
      , ftest  ( 0 )
    {}

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
        if ( 0 == ftest )
        {
          evt.reset();
          res = RET_ERROR_APC_PENDING;
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
    template <typename AUTO_LOCK_TYPE>
    void payload_cleanup()
    {
      AUTO_LOCK_TYPE lck(lock);
      ftest = 0;
      evt.set();
    }

    template <typename AUTO_LOCK_TYPE>
    void payload_impl()
    {
      testFT f = 0;
      {
        AUTO_LOCK_TYPE lck(lock);
        f = ftest;
      }
      f(res);
    }

    virtual void special_kernel_payload(PKNORMAL_ROUTINE*, void**, void**, void**)
    {
      typedef apc_auto_lock_type auto_lock_t;
      DbgPrint("kapc::special_kernel_payload() %p\n", KeGetCurrentThread());
      payload_impl<auto_lock_t>();
      payload_cleanup<auto_lock_t>();
    }

    virtual void regular_kernel_payload(PKNORMAL_ROUTINE*, void**, void**, void**)
    {
      typedef apc_auto_lock_type auto_lock_t;
      DbgPrint("kapc::regular_kernel_payload() %p\n", KeGetCurrentThread());
      payload_impl<auto_lock_t>();
    // cleanup is expected by regular_normal_payload()
    }

    virtual void regular_normal_payload(void*, void*, void*)
    {
      typedef auto_lock_type auto_lock_t;
      DbgPrint("kapc::regular_normal_payload() %p\n", KeGetCurrentThread());
      payload_impl<auto_lock_t>();
      payload_cleanup<auto_lock_t>();
    }

    virtual void rundown_payload()
    {
      typedef auto_lock_type auto_lock_t;
      DbgPrint("kapc::rundown_payload()\n");
      payload_cleanup<auto_lock_t>();
    }
  };


  template <typename APC_TYPE>
  class apc_tx
  {
    int&      res;
    testFT    func;
    PKTHREAD  kt_rx;

  public:
    apc_tx(int& i, testFT tf, PKTHREAD kt)
      : res    ( i )
      , func   ( tf )
      , kt_rx  ( kt )
    {}

    void operator()() const
    {
      apc_tx_routine(res, func, kt_rx);
    }

    static void apc_tx_routine(int& res, testFT func, PKTHREAD kt_rx)
    {
    //  DbgPrint("apc_tx : %p\n", KeGetCurrentThread());
      res = apc_tx_impl(func, kt_rx ? kt_rx : KeGetCurrentThread());
    }

  private:
    static int apc_tx_impl(testFT func, PKTHREAD kt_rx)
    {
      aux_::scoped_ptr<kapc> apc(new(std::nothrow) kapc);
      if ( !apc.get() )
      {
        return RET_ERROR_APC_ALLOC;
      }
      if ( !apc->spawn(APC_TYPE(), kt_rx, func) )
      {
        return RET_ERROR_APC_SPAWN;
      }
      if ( !NT_SUCCESS(apc->acquire()) )
      {
        return RET_ERROR_APC_ACQUIRE;
      }
      return apc->result();
    }

  };


  class apc_rx
  {
    aux_::kevent& evt;

  public:
    explicit apc_rx(aux_::kevent& e)
      : evt ( e )
    {}

    void operator()() const
    {
      apc_rx_routine(evt);
    }

    static void apc_rx_routine(aux_::kevent& evt)
    {
    //  DbgPrint("apc_rx : %p\n", KeGetCurrentThread());
      evt.acquire(STATUS_SUCCESS);
    }
  };


  // spawns a thread sending kAPCs to self
  class sending_apc_to_thread_self
  {
    aux_::thread_type  kt;
    int                status;

  public:
    sending_apc_to_thread_self()
      : status ( RET_ERROR_UNEXPECTED )
    {}

    ~sending_apc_to_thread_self()
    {
      acquire();
    }

    template <typename APC_TYPE>
    int spawn(testFT func)
    {
      if
      (
      #ifdef NTKE_KTHREAD
        ! NT_SUCCESS ( kt.spawn(typeaux::functee(&apc_tx<APC_TYPE>::apc_tx_routine, typeaux::make_ref(status), func, static_cast<PKTHREAD>(0))) )
      #else
        ! NT_SUCCESS ( kt.spawn(apc_tx<APC_TYPE>(status, func, 0)) )
      #endif
      )
      {
        return false;
      }
      return true;
    }

    void acquire()
    {
      kt.acquire(STATUS_SUCCESS);
    }

    int result() const
    {
      return status;
    }

  };


  // spawns a separate APC-receiving thread
  class sending_apc_to_another_thread
  {
    aux_::thread_type  kt_tx;
    aux_::thread_type  kt_rx;
    aux_::kevent       stop_evt;
    int                status;

  public:
    sending_apc_to_another_thread()
      : stop_evt ( false, aux_::kevent::MANUAL_RESET )
      , status   ( RET_ERROR_UNEXPECTED )
    {}

    ~sending_apc_to_another_thread()
    {
      acquire();
    }

    template <typename APC_TYPE>
    bool spawn(testFT func)
    {
      if
      (
      #ifdef NTKE_KTHREAD
        ! NT_SUCCESS ( kt_rx.spawn(typeaux::functee(&apc_rx::apc_rx_routine, typeaux::make_ref(stop_evt))) )
      #else
        ! NT_SUCCESS ( kt_rx.spawn(apc_rx(stop_evt)) )
      #endif
      )
      {
        return false;
      }
      if
      (
      #ifdef NTKE_KTHREAD
        ! NT_SUCCESS ( kt_tx.spawn(typeaux::functee(&apc_tx<APC_TYPE>::apc_tx_routine, typeaux::make_ref(status), func, kt_rx.native_type())) )
      #else
        ! NT_SUCCESS ( kt_tx.spawn(apc_tx<APC_TYPE>(status, func, kt_rx.native_type())) )
      #endif                                               
      )
      {
        return false;
      }
      return true;
    }

    void acquire()
    {
      kt_tx.acquire(STATUS_SUCCESS);
      stop_evt.set();
      kt_rx.acquire(STATUS_SUCCESS);
    }

    int result() const
    {
      return status;
    }

  };


  template <typename APC_TASK_TYPE>
  class apc_task
    : public aux_::task_bunch<APC_TASK_TYPE>
  {
    typedef aux_::task_bunch<APC_TASK_TYPE> base_type;

  public:
    apc_task()
    {}

    ~apc_task()
    {
      acquire();
    }

    template <typename APC_TYPE>
    bool spawn(std::size_t const& n, testFT f)
    {
      task_num = n;
      spawned = 0;
      task.reset(new(std::nothrow) task_type[task_num]);
      if ( !task.get() )
      {
        status = RET_ERROR_APC_ALLOC;
        return false;
      }
      for ( spawned = 0 ; spawned < task_num ; ++spawned )
      {
        if ( !task[spawned].spawn<APC_TYPE>(f) )
        {
          status = RET_ERROR_APC_TASK_SPAWN;
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


  template <typename APC_TYPE, typename APC_TASK_TYPE>
  int test_apc_impl(testFT tests[], std::size_t const& task_num)
  {
    for ( unsigned i = 0 ; tests[i] ; ++i )
    {
      DbgPrint("test_apc_impl() : spawning kapcs[%u] for test[%u]\n", unsigned(task_num), i);
      apc_task<APC_TASK_TYPE> task;
      if ( !task.spawn<APC_TYPE>(task_num, tests[i]) )
      {
        return RET_ERROR_APC_TASK_SPAWN;
      }
      task.acquire();

      for ( unsigned k = 0 ; k < task_num ; ++k )
      {
        if ( RET_SUCCESS != task[k].result() )
        {
          DbgPrint("test_apc_impl() ERROR : test[%u]=%i at kapc[%u]\n", i, task[k].result(), k);
          return task[k].result();
        }
      }
    }
    DbgPrint("test_apc_impl() : 0\n");
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

  int test_apc(testFT tests[])
  {
    using aux_::kapc;
    DbgPrint("test_apc() : special, rx-self\n");
    int status = test_apc_impl<kapc::special, sending_apc_to_thread_self>(tests, 1);
    if ( RET_SUCCESS == status )
    {
      DbgPrint("test_apc() : regular, rx-self\n");
      status = test_apc_impl<kapc::regular, sending_apc_to_thread_self>(tests, 1);
    }
    if ( RET_SUCCESS == status )
    {
      DbgPrint("test_apc() : special, rx-thread\n");
      status = test_apc_impl<kapc::special, sending_apc_to_another_thread>(tests, 1);
    }
    if ( RET_SUCCESS == status )
    {
      DbgPrint("test_apc() : regular, rx-thread\n");
      status = test_apc_impl<kapc::regular, sending_apc_to_another_thread>(tests, 1);
    }
    return status;
  }


  int test_apc_mt(testFT tests[])
  {
    using aux_::kapc;
    std::size_t const task_num = aux_::get_number_processors() * APC_FACTOR;
    DbgPrint("test_apc_mt() : task_num=%d\n", unsigned(task_num));
    if ( task_num > 1 )
    {
      DbgPrint("test_apc_mt() : special, rx-self\n");
      int status = test_apc_impl<kapc::special, sending_apc_to_thread_self>(tests, task_num);
      if ( RET_SUCCESS == status )
      {
        DbgPrint("test_apc_mt() : regular, rx-self\n");
        status = test_apc_impl<kapc::regular, sending_apc_to_thread_self>(tests, task_num);
      }
      if ( RET_SUCCESS == status )
      {
        DbgPrint("test_apc_mt() : special, rx-thread\n");
        status = test_apc_impl<kapc::special, sending_apc_to_another_thread>(tests, task_num);
      }
      if ( RET_SUCCESS == status )
      {
        DbgPrint("test_apc_mt() : regular, rx-thread\n");
        status = test_apc_impl<kapc::regular, sending_apc_to_another_thread>(tests, task_num);
      }
      return status;
    }
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


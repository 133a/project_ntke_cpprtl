/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include <cstddef>
#include <new>
#include "aux_apc.h"
#include "aux_cpu.h"
#include "aux_task.h"
#include "sync_type.h"
#include "thread_type.h"
#include "tests_aux.h"
#include "test_apc.h"


namespace cpprtl_tests
{
  enum
  {
    APC_FACTOR = 2
  };

  enum
  {
    RET_ERROR_APC_SPAWN       = -1301
  , RET_ERROR_APC_TASK_SPAWN  = -1302
  };


  template <typename PAYLOAD>
  class kapc
    : public aux_::kapc
  {
    typedef PAYLOAD                   payload_type;
    typedef fast_mutex_type           lock_type;
    typedef auto_fast_mutex_type      auto_lock_type;
    typedef apc_auto_fast_mutex_type  apc_auto_lock_type;

  private:
    lock_type     lock;
    event_type    evt;
    bool          spawned;
    payload_type  payload;

  public:
    kapc()
      : evt     ( false, event_type::MANUAL_RESET )
      , spawned ( false )
    {}

    virtual ~kapc()
    {
      auto_lock_type lck(lock);
    }

    template <typename PLD, typename APC_MODE>
    bool spawn(PLD const& pld, APC_MODE const& apc_mode, PKTHREAD kt)
    {
      auto_lock_type lck(lock);
      if ( !spawned )
      {
        payload = pld;
        evt.reset();
        return spawned = enqueue(apc_mode, kt, 0, 0, 0) == TRUE;
      }
      return false;
    }

    void acquire()
    {
      evt.acquire(STATUS_SUCCESS);
    }

    payload_type const& get_payload() const
    {
      return payload;
    }

  protected:
    template <typename AUTO_LOCK_TYPE>
    void payload_cleanup()
    {
      AUTO_LOCK_TYPE lck(lock);
      spawned = false;
      evt.set();
    }

    template <typename AUTO_LOCK_TYPE>
    void payload_impl()
    {
      payload();
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


  class apc_receiver
  {
    event_type& evt;

  public:
    explicit apc_receiver(event_type& e)
      : evt ( e )
    {}

    void operator()()
    {
    //  DbgPrint("apc_receiver : %p\n", KeGetCurrentThread());
      evt.acquire(STATUS_SUCCESS);
    }
  };


  template <typename APC_TYPE, typename APC_MODE, typename PAYLOAD>
  class apc_transmitter
  {
    typedef APC_TYPE apc_type;
    typedef APC_MODE apc_mode;
    typedef PAYLOAD  payload_type;

  private:       
    apc_type&     apc;
    int&          status;
    payload_type  payload;
    PKTHREAD      kt_rx;

  public:
    apc_transmitter(apc_type& a, int& st, payload_type const& pld, PKTHREAD rx)
      : apc     ( a )
      , status  ( st )
      , payload ( pld )
      , kt_rx   ( rx )
    {}

    void operator()()
    {
    //  DbgPrint("apc_transmitter : %p\n", KeGetCurrentThread());
      status = RET_SUCCESS;
      if ( !apc.spawn(payload, apc_mode(), kt_rx ? kt_rx : KeGetCurrentThread()) )
      {
        status = RET_ERROR_APC_SPAWN;
        return;
      }
      apc.acquire();
    }
  };


  //  spawns a thread sending kAPCs to self
  template <typename PAYLOAD, typename APC_MODE>
  class sending_apc_to_thread_self
  {
    typedef PAYLOAD             payload_type;
    typedef kapc<payload_type>  apc_type;
    typedef APC_MODE            apc_mode;

  private:       
    thread_type  kt;
    apc_type     apc;
    int          st;

  public:
    sending_apc_to_thread_self()
      : st ( RET_ERROR_UNEXPECTED )
    {}

    ~sending_apc_to_thread_self()
    {
      acquire();
    }

    template <typename PLD>
    bool spawn(PLD const& pld, std::size_t const& = 0)
    {
      typedef apc_transmitter<apc_type, apc_mode, PLD> apc_transmitter_type;
      return NT_SUCCESS(kt.spawn(apc_transmitter_type(apc, st, pld, 0)));
    }

    void acquire()
    {
      kt.acquire();
    }

    int status() const
    {
      return st;
    }

    payload_type const& get_payload() const
    {
      return apc.get_payload();
    }
  };


  //  spawns a separate APC-receiving thread
  template <typename PAYLOAD, typename APC_MODE>
  class sending_apc_to_another_thread
  {
    typedef PAYLOAD             payload_type;
    typedef kapc<payload_type>  apc_type;
    typedef APC_MODE            apc_mode;

  private:       
    thread_type  kt_rx;
    thread_type  kt_tx;
    event_type   stop_evt;
    apc_type     apc;
    int          st;

  public:
    sending_apc_to_another_thread()
      : stop_evt ( false, event_type::MANUAL_RESET )
      , st ( RET_ERROR_UNEXPECTED )
    {}

    ~sending_apc_to_another_thread()
    {
      acquire();
    }

    template <typename PLD>
    bool spawn(PLD const& pld, std::size_t const& = 0)
    {
      typedef apc_receiver apc_receiver_type;
      if ( !NT_SUCCESS(kt_rx.spawn(apc_receiver_type(stop_evt))) )
      {
        return false;
      }
      typedef apc_transmitter<apc_type, apc_mode, PLD> apc_transmitter_type;
      if ( !NT_SUCCESS(kt_tx.spawn(apc_transmitter_type(apc, st, pld, kt_rx.native_type()))) )
      {
        return false;
      }
      return true;
    }

    void acquire()
    {
      kt_tx.acquire();
      stop_evt.set();
      kt_rx.acquire();
    }

    int status() const
    {
      return st;
    }

    payload_type const& get_payload() const
    {
      return apc.get_payload();
    }
  };

}  // namespace cpprtl_tests


namespace cpprtl_tests
{

  template <typename APC_TASK_TYPE>
  int test_apc_impl(testFT tests[], std::size_t const& task_num)
  {
    typedef aux_::task_bunch<APC_TASK_TYPE> apc_task;
    for ( unsigned i = 0 ; tests[i] ; ++i )
    {
      DbgPrint("test_apc_impl() : spawning kapcs[%u] for test[%u]\n", unsigned(task_num), i);
      apc_task task;
      if ( !task.spawn(task_num, tests[i]) )
      {
        return RET_ERROR_APC_TASK_SPAWN;
      }
      task.acquire();

      for ( unsigned k = 0 ; k < task_num ; ++k )
      {
        int result = task[k].status();
        if ( RET_SUCCESS != result )
        {
          DbgPrint("test_apc_impl() ERROR : status=%i at kapc[%u]\n", result, k);
          return result;
        }
        result = task[k].get_payload().result();
        if ( RET_SUCCESS != result )
        {
          DbgPrint("test_apc_impl() ERROR : test[%u]=%i at kapc[%u]\n", i, result, k);
          return result;
        }
      }
    }
    DbgPrint("test_apc_impl() : 0\n");
    return RET_SUCCESS;
  }


  int test_apc(testFT tests[])
  {
    using aux_::kapc;
    DbgPrint("test_apc() : special, self-rx\n");
    int status = test_apc_impl<sending_apc_to_thread_self<test_payload, kapc::special> >(tests, 1);
    if ( RET_SUCCESS == status )
    {
      DbgPrint("test_apc() : regular, self-rx\n");
      status = test_apc_impl<sending_apc_to_thread_self<test_payload, kapc::regular> >(tests, 1);
    }
    if ( RET_SUCCESS == status )
    {
      DbgPrint("test_apc() : special, thread-rx\n");
      status = test_apc_impl<sending_apc_to_another_thread<test_payload, kapc::special> >(tests, 1);
    }
    if ( RET_SUCCESS == status )
    {
      DbgPrint("test_apc() : regular, thread-rx\n");
      status = test_apc_impl<sending_apc_to_another_thread<test_payload, kapc::regular> >(tests, 1);
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
      DbgPrint("test_apc_mt() : special, self-rx\n");
      int status = test_apc_impl<sending_apc_to_thread_self<test_payload, kapc::special> >(tests, task_num);
      if ( RET_SUCCESS == status )
      {
        DbgPrint("test_apc_mt() : regular, self-rx\n");
        status = test_apc_impl<sending_apc_to_thread_self<test_payload, kapc::regular> >(tests, task_num);
      }
      if ( RET_SUCCESS == status )
      {
        DbgPrint("test_apc_mt() : special, thread-rx\n");
        status = test_apc_impl<sending_apc_to_another_thread<test_payload, kapc::special> >(tests, task_num);
      }
      if ( RET_SUCCESS == status )
      {
        DbgPrint("test_apc_mt() : regular, thread-rx\n");
        status = test_apc_impl<sending_apc_to_another_thread<test_payload, kapc::regular> >(tests, task_num);
      }
      return status;
    }
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


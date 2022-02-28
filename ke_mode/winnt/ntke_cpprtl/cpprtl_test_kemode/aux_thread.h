//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef AUX_THREAD_H_
#define AUX_THREAD_H_


#include "ntddk.include.h"
#include "aux_sync.h"


namespace aux_
{

  namespace internal_
  {

    template <typename PAYLOAD>
    class thread_launcher
    {
      typedef PAYLOAD          payload_type;
      typedef thread_launcher  self_type;

      kevent evt;
      payload_type const& payload;

    public:
      thread_launcher(payload_type const& pld)
        : evt     ( false, kevent::MANUAL_RESET )
        , payload ( pld )
      {}

      void acquire()
      {
        evt.acquire(STATUS_SUCCESS);
      }

      static void thread_routine(void* arg)
      {
        ASSERT ( arg );
        ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
        KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);  // KeGetCurrentThread() at any IRQL
        self_type& launcher = *reinterpret_cast<self_type*>(arg);
        payload_type payload = launcher.payload;
        launcher.evt.set();
        payload();
        ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
        PsTerminateSystemThread(STATUS_SUCCESS);
      }
    };

  }  // namespace internal_


  class kthread
  {
    PKTHREAD kt;

    kthread(kthread const&);
    kthread& operator=(kthread const&);

  public:

    kthread()
      : kt(0)
    {}

    ~kthread()
    {
      detach();
    }

    PKTHREAD native_type() const
    {
      return kt;
    }

    void detach()
    {
      if ( kt )
      {
        ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
        ObDereferenceObject(kt);
        kt = 0;
      }
    }

    NTSTATUS acquire()
    {
      NTSTATUS status = STATUS_SUCCESS;
      if ( kt )
      {
        ASSERT ( KeGetCurrentThread() != kt );
        ASSERT ( KeGetCurrentIrql() <= APC_LEVEL );  // for continuous time-out
        status = KeWaitForSingleObject(kt, Executive, KernelMode, FALSE, 0);
      }
      return status;
    }

    NTSTATUS acquire(NTSTATUS const acq_st)
    {
      NTSTATUS st = STATUS_UNSUCCESSFUL;
      do
      {
        st = acquire();
      }
      while ( acq_st != st && NT_SUCCESS(st) );
      return st;
    }

    template <typename PAYLOAD>
    NTSTATUS spawn(PAYLOAD const& payload)
    {
      NTSTATUS status = STATUS_UNSUCCESSFUL;
      if ( !kt )
      {
        HANDLE ht;
        OBJECT_ATTRIBUTES oa;
        InitializeObjectAttributes(&oa, 0, OBJ_KERNEL_HANDLE, 0, 0);
    
        typedef internal_::thread_launcher<PAYLOAD> launcher_type;
        launcher_type launcher(payload);
        ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
        status = PsCreateSystemThread
        (
          &ht
        , THREAD_ALL_ACCESS
        , &oa
        , 0
        , 0
        , &launcher_type::thread_routine
        , reinterpret_cast<void*>(&launcher)
        );
        if ( NT_SUCCESS(status) )
        {
          ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
          ObReferenceObjectByHandle(ht, THREAD_ALL_ACCESS, 0, KernelMode, reinterpret_cast<PVOID*>(&kt), 0);
          ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
          ZwClose(ht);
          launcher.acquire();
        }
      }
      return status;
    }
  };

}  // namespace aux_


#endif  // include guard


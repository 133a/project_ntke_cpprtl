/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


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
    private:
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

      static void thread_start(void* arg)
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
    PKTHREAD thr;

    kthread(kthread const&);
    kthread& operator=(kthread const&);

  public:

    kthread()
      : thr(0)
    {}

    ~kthread()
    {
      detach();
    }

    PKTHREAD native_type() const
    {
      return thr;
    }

    void detach()
    {
      if ( thr )
      {
        ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
        ObDereferenceObject(thr);
        thr = 0;
      }
    }

    NTSTATUS acquire()
    {
      NTSTATUS status = STATUS_SUCCESS;
      if ( thr )
      {
        ASSERT ( KeGetCurrentThread() != thr );
        ASSERT ( KeGetCurrentIrql() <= APC_LEVEL );  // for continuous time-out
        status = KeWaitForSingleObject(thr, Executive, KernelMode, FALSE, 0);
      }
      return status;
    }

    NTSTATUS acquire(NTSTATUS const& acq_st)
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
      if ( !thr )
      {
        HANDLE hthread;
        OBJECT_ATTRIBUTES oa;
        InitializeObjectAttributes(&oa, 0, OBJ_KERNEL_HANDLE, 0, 0);
    
        typedef internal_::thread_launcher<PAYLOAD> launcher_type;
        launcher_type launcher(payload);
        ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
        status = PsCreateSystemThread
        (
          &hthread
        , THREAD_ALL_ACCESS
        , &oa
        , 0
        , 0
        , &launcher_type::thread_start
        , reinterpret_cast<void*>(&launcher)
        );
        if ( NT_SUCCESS(status) )
        {
          ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
          ObReferenceObjectByHandle(hthread, THREAD_ALL_ACCESS, 0, KernelMode, reinterpret_cast<PVOID*>(&thr), 0);
          ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
          ZwClose(hthread);
          launcher.acquire();
        }
      }
      return status;
    }
  };

}  // namespace aux_


#endif  // include guard


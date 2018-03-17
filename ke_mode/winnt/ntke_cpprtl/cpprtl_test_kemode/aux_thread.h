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

    template <typename FUNC>
    class thread_func_wrapper
    {
    private:
      typedef thread_func_wrapper self_type;

      kevent evt;
      FUNC const& func;

    public:
      thread_func_wrapper(FUNC const& f)
        : evt  ( false, kevent::MANUAL_RESET )
        , func ( f )
      {}

      void acquire()
      {
        evt.acquire(STATUS_SUCCESS);
      }

      static void kthread_start_func(void* arg)
      {
        ASSERT ( arg );
        ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
        KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);  // KeGetCurrentThread() at any IRQL
        self_type& wrapper = *reinterpret_cast<self_type*>(arg);
        FUNC func = wrapper.func;
        wrapper.evt.set();
        func();
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


    template <typename FUNC>
    NTSTATUS spawn(FUNC const& func)
    {
      NTSTATUS status = STATUS_UNSUCCESSFUL;
      if ( !thr )
      {
        HANDLE hthread;
        OBJECT_ATTRIBUTES oa;
        InitializeObjectAttributes(&oa, 0, OBJ_KERNEL_HANDLE, 0, 0);
    
        internal_::thread_func_wrapper<FUNC> tfw(func);
        ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
        status = 
          PsCreateSystemThread
          (
            &hthread
          , THREAD_ALL_ACCESS
          , &oa
          , 0
          , 0
          , &internal_::thread_func_wrapper<FUNC>::kthread_start_func
          , reinterpret_cast<void*>(&tfw)
          );
        if ( NT_SUCCESS(status) )
        {
          ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
          ObReferenceObjectByHandle(hthread, THREAD_ALL_ACCESS, 0, KernelMode, reinterpret_cast<PVOID*>(&thr), 0);
          ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );
          ZwClose(hthread);
          tfw.acquire();
        }
      }
      return status;
    }
  };

}  // namespace aux_


#endif  // include guard


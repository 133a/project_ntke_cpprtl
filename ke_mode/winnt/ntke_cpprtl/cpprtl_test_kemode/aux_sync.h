//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef AUX_SYNC_H_
#define AUX_SYNC_H_


#include "ntddk.include.h"


#define _NO_COPY_(CLASS_NAME)                 \
  private:                                    \
    CLASS_NAME(CLASS_NAME const&);            \
    CLASS_NAME& operator=(CLASS_NAME const&);


namespace aux_
{

//// event
  class kevent
  {
    KEVENT evt;  // make sure this is non-paged memory

  public:
    typedef enum
    {
      MANUAL_RESET = NotificationEvent,
      AUTO_RESET   = SynchronizationEvent,
    }
    reset_t;

  public:
    kevent(bool initial_state, reset_t event_type)
    {
      KeInitializeEvent(&evt, static_cast<EVENT_TYPE>(event_type), initial_state);  // any IRQL
    }

    NTSTATUS acquire()
    {
      ASSERT ( KeGetCurrentIrql() <= APC_LEVEL );  // for a continuous time-out
      return KeWaitForSingleObject(&evt, Executive, KernelMode, FALSE, 0);
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

    LONG set()
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );  // for Wait==FALSE
      return KeSetEvent(&evt, EVENT_INCREMENT, FALSE);
    }

    LONG reset()
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
      return KeResetEvent(&evt);
    }

    _NO_COPY_(kevent)
  };


//// spin lock
  class spin_lock
  {
    KSPIN_LOCK slck;  // make sure this is non-paged memory

  public:
    spin_lock()
    {
      KeInitializeSpinLock(&slck);  // any IRQL
    }

  public:
    void acquire(KIRQL& irql)
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
      KeAcquireSpinLock(&slck, &irql);
    }

    void release(KIRQL& irql)
    {
      ASSERT ( KeGetCurrentIrql() == DISPATCH_LEVEL );
      KeReleaseSpinLock(&slck, irql);
    }

    void acquire_at_dpc()
    {
      ASSERT ( KeGetCurrentIrql() >= DISPATCH_LEVEL );
      KeAcquireSpinLockAtDpcLevel(&slck);
    }

    void release_from_dpc()
    {
      ASSERT ( KeGetCurrentIrql() == DISPATCH_LEVEL );
      KeReleaseSpinLockFromDpcLevel(&slck);
    }

    _NO_COPY_(spin_lock)
  };


  class auto_spin_lock
  {
    spin_lock&  slck;
    KIRQL       irql;

  public:
    explicit auto_spin_lock(spin_lock& s)
      : slck ( s )
    {
      slck.acquire(irql);
    }

    ~auto_spin_lock()
    {
      slck.release(irql);
    }

    bool locked() const
    {
      return true;
    }

    _NO_COPY_(auto_spin_lock)
  };


  class dpc_auto_spin_lock
  {
    spin_lock&  slck;

  public:
    explicit dpc_auto_spin_lock(spin_lock& s)
      : slck ( s )
    {
      slck.acquire_at_dpc();
    }

    ~dpc_auto_spin_lock()
    {
      slck.release_from_dpc();
    }

    bool locked() const
    {
      return true;
    }

    _NO_COPY_(dpc_auto_spin_lock)
  };


//// mutex
  class kmutex
  {
    KMUTEX mtx;  // make sure this is non-paged memory

  public:
    kmutex()
    {
      KeInitializeMutex(&mtx, 0);  // any IRQL
    }

  public:
    NTSTATUS acquire()
    {
      ASSERT ( KeGetCurrentIrql() <= APC_LEVEL );  // for continuous time-out
      return KeWaitForSingleObject(&mtx, Executive, KernelMode, FALSE, 0);
    }

    void release()
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
      KeReleaseMutex(&mtx, FALSE);
    }

    _NO_COPY_(kmutex)
  };


  class auto_mutex
  {
    kmutex&   mtx;
    NTSTATUS  status;

  public:
    explicit auto_mutex(kmutex& m)
      : mtx    ( m )
      , status ( STATUS_UNSUCCESSFUL )
    {
      do 
      {
        status = mtx.acquire();
      }
      while ( !locked() && NT_SUCCESS(status) );
      ASSERT ( locked() );
    }

    ~auto_mutex()
    {
      mtx.release();
    }

  public:
    bool locked() const
    {
      return STATUS_SUCCESS == status;
    }

    _NO_COPY_(auto_mutex)
  };


//// fast mutex
  class fast_mutex
  {
    FAST_MUTEX fmtx;  // make sure this is non-paged memory

  public:
    fast_mutex()
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
      ExInitializeFastMutex(&fmtx);
    }

  public:
    void acquire()
    {
      ASSERT ( KeGetCurrentIrql() <= APC_LEVEL );
      ExAcquireFastMutex(&fmtx);
    }

    void release()
    {
      ASSERT ( KeGetCurrentIrql() == APC_LEVEL );
      ExReleaseFastMutex(&fmtx);
    }

    void acquire_unsafe()
    {
      ASSERT ( KeGetCurrentIrql() == APC_LEVEL );
      ExAcquireFastMutexUnsafe(&fmtx);
    }

    void release_unsafe()
    {
      ASSERT ( KeGetCurrentIrql() == APC_LEVEL );
      ExReleaseFastMutexUnsafe(&fmtx);
    }

    _NO_COPY_(fast_mutex)
  };


  class auto_fast_mutex
  {
    fast_mutex& fmtx;

  public:
    explicit auto_fast_mutex(fast_mutex& fm)
      : fmtx ( fm )
    {
      fmtx.acquire();
    }

    ~auto_fast_mutex()
    {
      fmtx.release();
    }

    bool locked() const
    {
      return true;
    }

    _NO_COPY_(auto_fast_mutex)
  };


  class apc_auto_fast_mutex
  {
    fast_mutex& fmtx;

  public:
    explicit apc_auto_fast_mutex(fast_mutex& fm)
      : fmtx ( fm )
    {
      fmtx.acquire_unsafe();
    }

    ~apc_auto_fast_mutex()
    {
      fmtx.release_unsafe();
    }

    bool locked() const
    {
      return true;
    }

    _NO_COPY_(apc_auto_fast_mutex)
  };

}  //  namespace aux_


#ifdef   _NO_COPY_
#  undef _NO_COPY_
#endif


#endif  // include guard


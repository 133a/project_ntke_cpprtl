/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef AUX_APC_H_
#define AUX_APC_H_


#include "ntddk.include.h"


extern "C"
{
  typedef enum _KAPC_ENVIRONMENT
  {
    OriginalApcEnvironment
  , AttachedApcEnvironment
  , CurrentApcEnvironment
  }
  KAPC_ENVIRONMENT;


  typedef
  VOID (*PKNORMAL_ROUTINE)
  (
    PVOID NormalContext
  , PVOID SystemArgument1
  , PVOID SystemArgument2
  );


  typedef
  VOID (*PKKERNEL_ROUTINE)
  (
    PKAPC              Apc
  , PKNORMAL_ROUTINE * NormalRoutine
  , PVOID            * NormalContext
  , PVOID            * SystemArgument1
  , PVOID            * SystemArgument2
  );


  typedef
  VOID (*PKRUNDOWN_ROUTINE)
  (
    PKAPC Apc
  );


  NTSTATUS KeInitializeApc
  (
    PKAPC              Apc
  , PKTHREAD           Thread
  , KAPC_ENVIRONMENT   Environment
  , PKKERNEL_ROUTINE   KernelRoutine
  , PKRUNDOWN_ROUTINE  RundownRoutine
  , PKNORMAL_ROUTINE   NormalRoutine
  , KPROCESSOR_MODE    ApcMode
  , PVOID              NormalContext
  );


  BOOLEAN KeInsertQueueApc
  (
    PKAPC      Apc
  , PVOID      SystemArgument1
  , PVOID      SystemArgument2
  , KPRIORITY  Increment
  );


  BOOLEAN KeRemoveQueueApc 
  (
    PKAPC Apc
  );
}


namespace aux_
{

  class kapc
    : protected ::KAPC
  {
  public:
    struct special {};
    struct regular {};

  public:
    virtual ~kapc() {}

  protected:
    virtual void special_kernel_payload(PKNORMAL_ROUTINE* pf_norm, void** pnorm_ctx, void** psys_ctx1, void** psys_ctx2) = 0;
    virtual void regular_kernel_payload(PKNORMAL_ROUTINE* pf_norm, void** pnorm_ctx, void** psys_ctx1, void** psys_ctx2) = 0;
    virtual void regular_normal_payload(void* norm_ctx, void* sys_ctx1, void* sys_ctx2) = 0;
    virtual void rundown_payload() = 0;

  protected:
    kapc() {}

    template <typename APC_MODE>
    BOOLEAN enqueue(APC_MODE const& apc_mode, PKTHREAD kt, void* norm_ctx, void* sys_ctx1, void* sys_ctx2)
    {
      init_apc(apc_mode, kt, norm_ctx);
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
      return KeInsertQueueApc(static_cast<KAPC*>(this), sys_ctx1, sys_ctx2, 0);
    }

    BOOLEAN remove()
    {
      return KeRemoveQueueApc(static_cast<KAPC*>(this));  // any IRQL
    }

  private:
    void init_apc(special const&, PKTHREAD kt, void* = 0)  // norm_ctx
    {
      KeInitializeApc  // any IRQL
      (
        static_cast<KAPC*>(this)
      , kt
      , CurrentApcEnvironment
      , &kapc::special_kernel_apc_routine
      , &kapc::rundown_apc_routine
      , 0
      , KernelMode
      , 0  // norm_ctx isn't stored since regular_normal_apc_routine is never invoked
      );
    }

    void init_apc(regular const&, PKTHREAD kt, void* norm_ctx)
    {
      KeInitializeApc  // any IRQL
      (
        static_cast<KAPC*>(this)
      , kt
      , CurrentApcEnvironment
      , &kapc::regular_kernel_apc_routine
      , &kapc::rundown_apc_routine
      , &kapc::regular_normal_apc_routine
      , KernelMode
      , norm_ctx
      );
    }

  private:
    static void special_kernel_apc_routine(KAPC* pa, PKNORMAL_ROUTINE* pf_norm, void** pnorm_ctx, void** psys_ctx1, void** psys_ctx2)
    {
      ASSERT ( KeGetCurrentIrql() == APC_LEVEL );  // for a special kAPC routine
      static_cast<kapc*>(pa)->special_kernel_payload(pf_norm, pnorm_ctx, psys_ctx1, psys_ctx2);
    }

    static void regular_kernel_apc_routine(KAPC* pa, PKNORMAL_ROUTINE* pf_norm, void** pnorm_ctx, void** psys_ctx1, void** psys_ctx2)
    {
      ASSERT ( KeGetCurrentIrql() == APC_LEVEL );  // for a regular kAPC routine
      kapc* apc = static_cast<kapc*>(pa);
      apc->regular_kernel_payload(pf_norm, pnorm_ctx, psys_ctx1, psys_ctx2);
      *pnorm_ctx = reinterpret_cast<void*>(apc);  // tamper the temporary norm_ctx by kapc* for the regular_normal_apc_routine to be able to access our kapc object
    }

    static void regular_normal_apc_routine(void* norm_ctx, void* sys_ctx1, void* sys_ctx2)
    {
      ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );  // for a normal kAPC routine
      ASSERT ( norm_ctx );  // assumed kapc* is stored here by regular_kernel_apc_routine
      kapc* apc = reinterpret_cast<kapc*>(norm_ctx);
      apc->regular_normal_payload(apc->KAPC::NormalContext, sys_ctx1, sys_ctx2);  // untamper the normal_ctx here
    }

    static void rundown_apc_routine(KAPC* pa)
    {
      ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );  // for an APC rundown routine
      static_cast<kapc*>(pa)->rundown_payload();
    }

  private:
    kapc(kapc const&);
    kapc& operator=(kapc const&);
  };


}  // namespace aux_


#endif  // include guard


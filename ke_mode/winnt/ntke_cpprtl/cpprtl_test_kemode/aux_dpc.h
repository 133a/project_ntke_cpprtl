/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef AUX_DPC_H_
#define AUX_DPC_H_


#include "ntddk.include.h"


namespace aux_
{

  class kdpc
  {
    KDPC dpc;

  public:
    virtual ~kdpc() {}

    void set_processor(char const& pn)
    {
      KeSetTargetProcessorDpc(&dpc, pn);
    }

  protected:
    virtual void payload(void* sys_ctx1, void* sys_ctx2) = 0;

  protected:
    kdpc()
    {
      KeInitializeDpc(&dpc, &kdpc::dpc_routine, reinterpret_cast<void*>(this));  //  any IRQL
    }

    BOOLEAN enqueue(void* sys_ctx1, void* sys_ctx2)
    {
      return KeInsertQueueDpc(&dpc, sys_ctx1, sys_ctx2);  //  any IRQL
    }

    BOOLEAN remove()
    {
      return KeRemoveQueueDpc(&dpc);  // any IRQL
    }

  private:
    static void dpc_routine(KDPC*, void* ctx, void* sys_ctx1, void* sys_ctx2)
    {
      ASSERT ( KeGetCurrentIrql() == DISPATCH_LEVEL );  //  for a DPC routine
      if ( kdpc* dpc = reinterpret_cast<kdpc*>(ctx) )
      {
        dpc->payload(sys_ctx1, sys_ctx2);
      }
    }

  private:
    kdpc(kdpc const&);
    kdpc& operator=(kdpc const&);
  };


}  //  namespace aux_


#endif // include guard


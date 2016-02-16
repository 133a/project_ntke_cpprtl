/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef AUX_WORKITEM_H_
#define AUX_WORKITEM_H_


#include "ntddk.include.h"


namespace aux_
{

  class work_item
  {
    WORK_QUEUE_ITEM wqi;

  public:
    virtual ~work_item() {}

  protected:
    virtual void payload() = 0;

  protected:
    work_item()
    {
      ExInitializeWorkItem(&wqi, &work_item::wqi_routine, reinterpret_cast<void*>(this));  //  any IRQL
    }

    void enqueue()
    {
      ASSERT ( KeGetCurrentIrql() <= DISPATCH_LEVEL );
      ExQueueWorkItem(&wqi, DelayedWorkQueue);
    }

  private:
    static void wqi_routine(void* ctx)
    {
      ASSERT ( KeGetCurrentIrql() == PASSIVE_LEVEL );  //  for a work-item routine
      if ( work_item* wi = reinterpret_cast<work_item*>(ctx) )
      {
        wi->payload();
      }
    ////  be aware of a race 'the driver unloading vs this execution point'
    }

  private:
    work_item(work_item const&);
    work_item& operator=(work_item const&);
  };


}  //  namespace aux_


#endif // include guard


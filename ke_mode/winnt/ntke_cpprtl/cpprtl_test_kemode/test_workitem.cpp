/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include "tests_aux.h"
#include "aux_workitem.h"
#include "aux_sync.h"
#include "test_workitem.h"


namespace
{

  class work_item
    : public aux_::work_item
  {
    typedef aux_::spin_lock       lock_type;
    typedef aux_::auto_spin_lock  auto_lock_type;

  public:
    typedef cpprtl_tests::testFT testFT;

  private:
    lock_type     lock;
    aux_::kevent  evt;
    int           res;
    testFT        ftest;

  public:
    work_item()
      : evt    ( false, aux_::kevent::MANUAL_RESET )
      , res    ( cpprtl_tests::RET_ERROR_UNEXPECTED )
      , ftest  ( 0 )
    {}

    virtual ~work_item()
    {
      auto_lock_type lck(lock);
    }

    BOOLEAN spawn(testFT f)
    {
      bool do_enqueue = false;
      {
        auto_lock_type lck(lock);
        if ( lck.locked() && 0 == ftest )
        {
          evt.reset();
          res = cpprtl_tests::RET_ERROR_UNEXPECTED;
          ftest = f;
          do_enqueue = true;
        }
      }
      if ( do_enqueue )
      {
        enqueue();
        return TRUE;
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
    virtual void payload()
    {
      DbgPrint("work_item::payload() %d\n", KeGetCurrentThread());
      if ( ftest )
      {
        ftest(res);
      }
      auto_lock_type lck(lock);
      if ( lck.locked() )
      {
        ftest = 0;
        evt.set();
      }
    }

  };

}


namespace cpprtl_tests
{

  //  run in the queued work item
  int test_workitem(testFT test_funcs[])
  {
    work_item wi;
    for ( int i = 0 ; test_funcs[i] ; ++i )
    {
      if ( !wi.spawn(test_funcs[i]) )
      {
        return RET_ERROR_WORK_ITEM_SPAWN;
      }
      if ( !NT_SUCCESS(wi.acquire()) )
      {
        return RET_ERROR_WORK_ITEM_ACQUIRE;
      }
      if ( RET_SUCCESS != wi.result() )
      {
        DbgPrint("cpprtl_tests::test_workitem() : test_func %d ret %d\n", i, wi.result());
        return wi.result();
      }
    }
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


namespace
{
  using cpprtl_tests::WQI_NUM;


  class wi_task
  {
    work_item   wi         [WQI_NUM];
    int       (&wi_status) [WQI_NUM];

  public:
    explicit wi_task(int (&st) [WQI_NUM])
      : wi_status ( st )
    {}

    bool spawn(cpprtl_tests::testFT f)
    {
      for ( int i = 0 ; i < WQI_NUM ; ++i )
      {
        if ( ! wi[i].spawn(f) )
        {
          wi_status[i] = cpprtl_tests::RET_ERROR_WORK_ITEM_SPAWN;
          return false;
        }
        wi_status[i] = STATUS_SUCCESS;
      }
      return true;
    }

    bool acquire()
    {
      bool ok = true;
      for ( int i = 0 ; i < WQI_NUM && NT_SUCCESS(wi_status[i]) ; ++i )
      {
        if ( !NT_SUCCESS(wi[i].acquire()) )
        {
          wi_status[i] = cpprtl_tests::RET_ERROR_WORK_ITEM_ACQUIRE;
          ok = false;
          continue;
        }
        wi_status[i] = wi[i].result();
      }
      return ok;
    }
  };


  class wi_task_auto_acquire
  {
    wi_task* wi;

  public:
    explicit wi_task_auto_acquire(wi_task* const t)
      : wi ( t )
    {}

    void release()
    {
      wi = 0;
    }

    ~wi_task_auto_acquire()
    {
      if ( wi )
      {
        wi->acquire();
      }
    }
  };

}


namespace cpprtl_tests
{

  int test_workitem_mt(testFT test_funcs[])
  {
    int wi_status [WQI_NUM] = { STATUS_UNSUCCESSFUL };
    wi_task wi(wi_status);

    for ( int i = 0 ; test_funcs[i] ; ++i )
    {
      wi_task_auto_acquire acq(&wi);  // spawn guard
      if ( ! wi.spawn(test_funcs[i]) )
      {
        return RET_ERROR_WORK_ITEM_TASK_SPAWN;
      }
      acq.release();
      if ( ! wi.acquire() )
      {
        return RET_ERROR_WORK_ITEM_TASK_ACQUIRE;
      }

      for ( int k = 0 ; k < WQI_NUM ; ++k)
      {
        if ( RET_SUCCESS != wi_status[k] )
        {
          DbgPrint("cpprtl_tests::test_workitem_mt() : test_func %d ret %d\n", i, wi_status[k]);
          return wi_status[k];
        }
      }
    }
    return RET_SUCCESS;
  }

}  //  namespace cpprtl_tests


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "ntddk.include.h"
#include "tests_aux.h"
#include "test_irql.h"


namespace
{

  class auto_irql_raiser  // simple scope-based IRQL controller
  {
    KIRQL saved_irql;

  public:

    explicit auto_irql_raiser(KIRQL const& new_irql)
    {
      ASSERT ( KeGetCurrentIrql() <= new_irql );  // KeRaiseIrql()
      KeRaiseIrql(new_irql, &saved_irql);
    }

    ~auto_irql_raiser()
    {
      KeLowerIrql(saved_irql);  //  any IRQL 
    }

  private:  //  what for a copying can be needed ?
    auto_irql_raiser(auto_irql_raiser const&);
    auto_irql_raiser& operator=(auto_irql_raiser const&);
  };



  KIRQL test_at_irql[] = 
  {
    PASSIVE_LEVEL
  , APC_LEVEL
  , DISPATCH_LEVEL
  };

}


namespace cpprtl_tests
{

  int test_irql(testFT test_funcs[])  ////  run in the current thread at various IRQLs
  {
    KIRQL const current_irql = KeGetCurrentIrql();
    try
    {
      for ( unsigned k = 0 ; k < sizeof(test_at_irql) / sizeof(test_at_irql[0]) ; ++k )
      {
        auto_irql_raiser auto_level(test_at_irql[k]);
        for ( int i = 0, res = RET_ERROR_UNEXPECTED ; test_funcs[i] ; ++i )
        {
          test_funcs[i](res);
          if ( RET_SUCCESS != res )
          {
            DbgPrint("cpprtl_tests::test_irql() : test_func %d ret %d at IRQL %d\n", i, res, int(KeGetCurrentIrql()));
            return res;
          }
        }
      }
    }
    catch ( ... )
    {
      ASSERT ( KeGetCurrentIrql() == current_irql );
      return RET_ERROR_UNEXPECTED;
    }
    ASSERT ( KeGetCurrentIrql() == current_irql );  //  check the auto_irql_raiser has us put at the initial state
    return RET_SUCCESS;
  }

}  //  namespace cpprtl_tests


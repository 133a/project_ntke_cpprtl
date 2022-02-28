//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "ntddk.include.h"
#include <cstddef>
#include "aux_irql.h"
#include "tests_aux.h"
#include "test_irql.h"


namespace
{
  KIRQL test_at_irql[] = 
  {
    PASSIVE_LEVEL
  , APC_LEVEL
  , DISPATCH_LEVEL
  };

  enum
  {
    RET_ERROR_INCORRECT_IRQL = -1501
  };
}


namespace cpprtl_tests
{

  //  run in the current thread at various IRQLs
  int test_irql(test_type const tests[])
  {
    DbgPrint("test_irql()\n");
    KIRQL const initial_irql = KeGetCurrentIrql();

    {
      aux_::auto_irql_raiser irql(DISPATCH_LEVEL);
      if ( KeGetCurrentIrql() != DISPATCH_LEVEL )  // check the auto_irql_raiser has us up
      {
        KeLowerIrql(initial_irql);  // any IRQL
        return RET_ERROR_INCORRECT_IRQL;
      }
    }
    if ( KeGetCurrentIrql() != initial_irql )  // check the auto_irql_raiser has us put at the initial level
    {
      KeLowerIrql(initial_irql);  // any IRQL
      return RET_ERROR_INCORRECT_IRQL;
    }

    try
    {
      for ( unsigned i = 0 ; tests[i] ; ++i )
      {
        for ( unsigned k = 0 ; k < sizeof(test_at_irql) / sizeof(test_at_irql[0]) ; ++k )
        {
          int res = RET_ERROR_UNEXPECTED;
          {
            aux_::auto_irql_raiser irql(test_at_irql[k]);
            tests[i](res);
            DbgPrint("test_irql() : test[%u]=%i at IRQL=%u\n", i, res, unsigned(KeGetCurrentIrql()));  // DbgPrint() IRQL < DIRQL
          }
          if ( RET_SUCCESS != res )
          {
            return res;
          }
        }
      }
    }
    catch (...)
    {
      ASSERT ( KeGetCurrentIrql() == initial_irql );
      return RET_ERROR_UNEXPECTED;
    }

    ASSERT ( KeGetCurrentIrql() == initial_irql );
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


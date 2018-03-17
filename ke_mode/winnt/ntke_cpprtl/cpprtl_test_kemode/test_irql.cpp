/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


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
}


namespace cpprtl_tests
{

  //  run in the current thread at various IRQLs
  int test_irql(testFT tests[])
  {
    DbgPrint("test_irql()\n");
    KIRQL const initial_irql = KeGetCurrentIrql();
    try
    {
      for ( unsigned k = 0 ; k < sizeof(test_at_irql) / sizeof(test_at_irql[0]) ; ++k )
      {
        aux_::auto_irql_raiser irql(test_at_irql[k]);
        for ( unsigned i = 0 ; tests[i] ; ++i )
        {
          int res = RET_ERROR_UNEXPECTED;
          tests[i](res);
          DbgPrint("test_irql() : test[%u]=%i at IRQL=%u\n", i, res, unsigned(KeGetCurrentIrql()));  // DbgPrint() IRQL < DIRQL
          if ( RET_SUCCESS != res )
          {
            return res;
          }
        }
      }
    }
    catch ( ... )
    {
      ASSERT ( KeGetCurrentIrql() == initial_irql );
      return RET_ERROR_UNEXPECTED;
    }
    ASSERT ( KeGetCurrentIrql() == initial_irql );  // check the auto_irql_raiser has us put at the initial level
    return RET_SUCCESS;
  }

}  // namespace cpprtl_tests


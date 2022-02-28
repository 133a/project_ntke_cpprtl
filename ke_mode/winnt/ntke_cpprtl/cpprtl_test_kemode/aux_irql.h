//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef AUX_IRQL_H_
#define AUX_IRQL_H_


#include "ntddk.include.h"


namespace aux_
{
  class auto_irql_raiser  // scope-based IRQL controller
  {
    KIRQL saved_irql;

  public:
    explicit auto_irql_raiser(KIRQL const new_irql)
    {
      ASSERT ( KeGetCurrentIrql() <= new_irql );  // KeRaiseIrql()
      KeRaiseIrql(new_irql, &saved_irql);
    }

    ~auto_irql_raiser()
    {
      KeLowerIrql(saved_irql);  // any IRQL 
    }

  private:  // turn copying off
    auto_irql_raiser(auto_irql_raiser const&);
    auto_irql_raiser& operator=(auto_irql_raiser const&);
  };
}


#endif  // include guard


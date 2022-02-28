//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef CHECK_IRQL_H_
#define CHECK_IRQL_H_


#ifdef NT_KERNEL_MODE
    #define IRQL_CHECK(COMPARE_TO_VAL) ASSERT(KeGetCurrentIrql() COMPARE_TO_VAL);
    #define IRQL_IS(COMPARE_TO_VAL) (KeGetCurrentIrql() COMPARE_TO_VAL)
#else
    #define IRQL_CHECK(COMPARE_TO_VAL)
    #define IRQL_IS(COMPARE_TO_VAL) (true)
#endif  // NT_KERNEL_MODE


#endif  // include guard


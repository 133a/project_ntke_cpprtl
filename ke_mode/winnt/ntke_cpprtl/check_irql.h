/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef CHECK_IRQL_H_
#define CHECK_IRQL_H_


#ifdef NT_KERNEL_MODE
    #define IRQL_CHECK(REL_TO_VAL) ASSERT(KeGetCurrentIrql() REL_TO_VAL);
    #define IRQL_REL(REL_TO_VAL) (KeGetCurrentIrql() REL_TO_VAL)
#else
    #define IRQL_CHECK(REL_TO_VAL)
    #define IRQL_REL(REL_TO_VAL) (true)
#endif // NT_KERNEL_MODE


#endif // include guard


module.type    = sys
module.name    = cpprtl_test

 
module.definitions   += TEST_GSTATIC TEST_EH TEST_RTTI TEST_STL \
                        TEST_WORKITEM TEST_DPC TEST_APC TEST_THREAD


module.dir.include    = $(project.dir.root)/ke_mode/winnt  \
                        $(project.dir.root)/ke_mode/winnt/ntke_cpprtl

module.sources        = main.cpp \
                        aux_cpu.cpp \
                        run_tests.cpp \
                        test_irql.cpp \
                        test_workitem.cpp \
                        test_dpc.cpp \
                        test_apc.cpp \
                        test_thread.cpp

module.entry_point    = DriverEntry # should be overridden by ntke_cpprtl.lib.mak


module.lib.sdk                   = ntoskrnl hal libcntpr
module.lib.sdk.wdk8.1.x86        = BufferOverflowK
module.lib.sdk.wdk8.1.x64        = BufferOverflowK
module.lib.sdk.wdk8.1.arm        = bufferoverflowfastfailk
module.lib.sdk.wdk10.arm         = bufferoverflowfastfailk
module.lib.sdk.ewdk16299-km.arm  = bufferoverflowfastfailk


module.lib.depend  = ke_mode/winnt/ntke_cpprtl/eh/eh_test_suite/ntke_eh_test_suite  \
                     ke_mode/winnt/ntke_cpprtl/rtti/rtti_test_suite/ntke_rtti_test_suite \
                     ke_mode/winnt/ntke_cpprtl/gstatic_test_suite/ntke_gstatic_test_suite  \
                     ke_mode/winnt/ntke_cpprtl/stl_test_suite/ntke_stl_test_suite
                  

module.no_signing = 1


include $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/ntke_cpprtl.lib.mak


module.type    = sys
module.name    = cpprtl_test

 
module.definitions   += TEST_GSTATIC TEST_EH TEST_RTTI TEST_STL \
                        TEST_WORKITEM TEST_DPC TEST_APC TEST_THREADS

module.path.include   = $(project.dir.root)/ke_mode/winnt  \
                        $(project.dir.root)/ke_mode/winnt/ntke_cpprtl

module.sources        = main.cpp run_tests.cpp test_irql.cpp test_workitem.cpp test_thread.cpp test_dpc.cpp test_apc.cpp

module.entry_point    = DriverEntry # should be overrided by ntke_cpprtl.lib.mak


module.lib.sdk             = ntoskrnl.lib hal.lib libcntpr.lib
module.lib.sdk.wdk8.1.x86  = BufferOverflowK.lib
module.lib.sdk.wdk8.1.x64  = BufferOverflowK.lib
module.lib.sdk.wdk8.1.arm  = bufferoverflowfastfailk.lib
module.lib.sdk.wdk10.arm   = bufferoverflowfastfailk.lib


module.lib.depend  = ke_mode/winnt/ntke_cpprtl/eh/eh_test_suite/ntke_eh_test_suite  \
                     ke_mode/winnt/ntke_cpprtl/rtti/rtti_test_suite/ntke_rtti_test_suite \
                     ke_mode/winnt/ntke_cpprtl/gstatic_test_suite/ntke_gstatic_test_suite  \
                     ke_mode/winnt/ntke_cpprtl/stl_test_suite/ntke_stl_test_suite
                  
module.lib.depend += $(if $(filter NTKE_KTHREAD,$(module.definitions)),ke_mode/winnt/ntke/ntke,)


module.no_signing = 1
module.cert.file.pfx = ./sign/cpprtl_test.sign.pfx


include $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/ntke_cpprtl.lib.mak


module.name = cpprtl_test
module.type = sys
module.no_signing = 1

 
module.definitions   += TEST_GSTATIC TEST_EH TEST_RTTI TEST_STL \
                        TEST_IRQL TEST_WORKITEM TEST_DPC TEST_APC TEST_THREAD


module.sources  = main.cpp \
                  aux_cpu.cpp \
                  run_tests.cpp \
                  test_irql.cpp \
                  test_workitem.cpp \
                  test_dpc.cpp \
                  test_apc.cpp \
                  test_thread.cpp


ifdef toolset.not_supported.RTC_and_GS
  module.entry_point = DriverEntry
else
  module.entry_point = GsDriverEntry
endif


module.lib.sdk          = ntoskrnl hal libcntpr
ifndef toolset.not_supported.RTC_and_GS
  module.lib.sdk.x86    = BufferOverflowK 
  module.lib.sdk.x64    = BufferOverflowK
  module.lib.sdk.arm    = bufferoverflowfastfailk
  module.lib.sdk.arm64  = bufferoverflowfastfailk
endif
module.lib.sdk.wdk6001.nt5.2-x64  = sehupd
module.lib.sdk.wdk7600.nt5.2-x64  = sehupd


module.lib.depend  = ke_mode/winnt/ntke_cpprtl/eh/eh_test_suite/ntke_eh_test_suite \
                     ke_mode/winnt/ntke_cpprtl/rtti/rtti_test_suite/ntke_rtti_test_suite \
                     ke_mode/winnt/ntke_cpprtl/gstatic_test_suite/ntke_gstatic_test_suite \
                     ke_mode/winnt/ntke_cpprtl/stl_test_suite/ntke_stl_test_suite \
                     ke_mode/winnt/ntke_cpprtl/ntke_cpprtl

                  
module.dir.include  = $(project.dir.root)/ke_mode/winnt \
                      $(project.dir.root)/ke_mode/winnt/ntke_cpprtl \
                      $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/std_incl

module.sources     = eh_test_suite.cpp \
                     test00.cpp \
                     test01.cpp test02.cpp test03.cpp test04.cpp test05.cpp test06.cpp test07.cpp test08.cpp test09.cpp test10.cpp \
                     test11.cpp test12.cpp test13.cpp test14.cpp test15.cpp test16.cpp test17.cpp test18.cpp test19.cpp test20.cpp \
                     test21.cpp test22.cpp test23.cpp test24.cpp test25.cpp test26.cpp \
                     test51.cpp test52.cpp test53.cpp test54.cpp \
                     test100.cpp


module.dir.include        += $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh

module.dir.include.x86    += $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/frame_based

module.dir.include.x64    += $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/table_based \
                             $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/table_based/_x64 

module.dir.include.arm    += $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/table_based \
                             $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/table_based/_arm

module.dir.include.arm64  += $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/table_based \
                             $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/eh/table_based/_arm64


module.EHa = 1


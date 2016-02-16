module.name = ntke_stl_test_suite
module.type = syslib


version.stlport = 5.2.1
include $(project.dir.root)/3rd_party/STLport/$(version.stlport)/stlport.mak


module.path.include  =  $(include.dir.stlport) \
                        $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/std_incl \
                        $(project.dir.root)/ke_mode/winnt/ntke_cpprtl


ifndef module.EHcpp
module.EHcpp = a
endif


include ./module.src.mak


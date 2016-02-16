module.name = ntke_rtti_test_suite
module.type = syslib


module.path.include  = $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/std_incl


ifndef module.EHcpp
module.EHcpp = a
endif


include ./module.src.mak


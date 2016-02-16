module.name = ntke_cpprtl
module.type = syslib

module.path.base = .


#############################################################################
###    common rtlk stuff
#############################################################################
module.sources       = rtlk_init.cpp \
                       rtlk_entry.cpp \
                       rtlk_heap.cpp \
                       rtlk_new.cpp \
                       rtlk_gstatic.cpp \
                       rtlk_std.cpp \
                       rtlk_c.cpp

module.path.include  = $(project.dir.root)/ke_mode/winnt/ntke_cpprtl

module.EHcpp = a
module.RTTI  = 1



#############################################################################
###    eh stuff
#############################################################################
include ./module.src.eh.mak
module.sources.arm += eh_restore_context.arm.asm  # ke-specific


#############################################################################
###    rtti stuff
#############################################################################
include ./module.src.rtti.mak


module.name = ntke_cpprtl
module.type = syslib

module.dir.base = .


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

module.dir.include   = $(project.dir.root)/ke_mode/winnt/ntke_cpprtl


#############################################################################
###    eh stuff
#############################################################################
include ./module.src.eh.mak
module.sources.arm += eh_restore_context.arm.asm  # ke-specific


#############################################################################
###    rtti stuff
#############################################################################
include ./module.src.rtti.mak


module.sources         = eh_engine.cpp \
                         eh_stack_walk.cpp \
                         eh_array_support.cpp \
                         eh_aux.cpp

module.sources.x86    += eh_funclet.cpp \
                         eh_seh_reg.cpp \
                         eh_safeseh.x86.asm

ifdef toolset.not_supported.thiscall
  module.sources.x86  += eh_array_support_thunk.x86.asm
endif


ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif

module.dir.include = $(module.dir.base)  $(module.dir.base)/frame_based  $(module.dir.base)/frame_based/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.dir.sources = $(module.dir.base)  $(module.dir.base)/frame_based  $(module.dir.base)/frame_based/_$(target.arch)


module.sources = \
    eh_array_support.cpp \
    eh_aux.cpp \
    eh_engine.cpp \
    eh_stack_walk.cpp


module.sources.x64 = \
    eh_funclet.x64.asm \
    eh_context.x64.asm \
    eh_ep.x64.asm


module.sources.arm = \
    eh_funclet.arm.asm \
    eh_context.arm.asm


module.sources.arm64 = \
    eh_funclet.arm64.asm \
    eh_context.arm64.asm


ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


module.dir.include  = $(module.dir.base)  $(module.dir.base)/table_based  $(module.dir.base)/table_based/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.dir.sources  = $(module.dir.base)  $(module.dir.base)/table_based  $(module.dir.base)/table_based/_$(target.arch)


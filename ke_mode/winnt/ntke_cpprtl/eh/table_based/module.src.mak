module.sources = \
    eh_msvc_entry_points.cpp \
    eh_msvc_array_support.cpp \
    eh_msvc_internal_data_aux.cpp \
    eh_aux.cpp \
    eh_engine.cpp


module.sources.x64 = \
    eh_stack_walker.cpp \
    eh_execute_handler.x64.asm \
    eh_invoke_funclet.x64.asm \
    eh_restore_context.x64.asm


module.sources.arm = \
    eh_stack_walker.cpp \
    eh_execute_handler.arm.asm \
    eh_invoke_funclet.arm.asm


module.sources.arm64 = \
    eh_stack_walker.cpp \
    eh_execute_handler.arm64.asm \
    eh_invoke_funclet.arm64.asm \
    eh_restore_context.arm64.asm


ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


module.dir.include  = $(module.dir.base)  $(module.dir.base)/table_based  $(module.dir.base)/table_based/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.dir.sources  = $(module.dir.base)  $(module.dir.base)/table_based  $(module.dir.base)/table_based/_$(target.arch)


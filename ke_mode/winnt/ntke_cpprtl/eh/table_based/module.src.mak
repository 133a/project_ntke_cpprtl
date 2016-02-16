module.sources = \
    eh_msvc_entry_points.cpp \
    eh_msvc_array_support.cpp \
    eh_msvc_internal_data_aux.cpp \
    eh_aux.cpp \
    eh_engine.cpp


module.sources.x64 = \
    eh_stack_walker.cpp \
    eh_execute_handler.x64.asm \
    eh_restore_context.x64.asm \
    eh_invoke_funclet.x64.asm


module.sources.arm = \
    eh_stack_walker.cpp \
    eh_invoke_funclet.arm.asm \
    eh_execute_handler.arm.asm


ifndef module.path.base
$(error $(module.name) module.path.base is not defined)
endif


module.path.include  = $(module.path.base)  $(module.path.base)/table_based  $(module.path.base)/table_based/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.path.sources  = $(module.path.base)  $(module.path.base)/table_based  $(module.path.base)/table_based/_$(target.arch)


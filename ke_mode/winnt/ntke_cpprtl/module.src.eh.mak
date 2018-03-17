ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


#############################################################################
###    eh stuff
#############################################################################
module.sources            += eh_aux.cpp \
                             eh_msvc_internal_data_aux.cpp \
                             eh_msvc_array_support.cpp

module.dir.include        += $(module.dir.base)/eh
module.dir.sources        += $(module.dir.base)/eh



###############################
###    eh stuff x86
###############################
module.sources.x86        += eh_msvc_entry_points.cpp \
                             eh_msvc_internal_data.cpp \
                             eh_engine.cpp \
                             eh_exception_registration.cpp \
                             eh_stack_walker.cpp \
                             eh_except_list.x86.asm

ifdef toolset.not_supported.thiscall
  module.sources.x86      += eh_msvc_array_support_thunk.x86.asm
endif
module.dir.include.x86    += $(module.dir.base)/eh/frame_based  $(module.dir.base)/eh/frame_based/_x86
module.dir.sources.x86    += $(module.dir.base)/eh/frame_based  $(module.dir.base)/eh/frame_based/_x86



###############################
###    eh stuff x64
###############################
module.sources.x64        += eh_msvc_entry_points.cpp \
                             eh_engine.cpp \
                             eh_stack_walker.cpp \
                             eh_execute_handler.x64.asm \
                             eh_invoke_funclet.x64.asm \
                             eh_restore_context.x64.asm

module.dir.include.x64    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_x64
module.dir.sources.x64    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_x64



###############################
###    eh stuff arm
###############################
module.sources.arm        += eh_msvc_entry_points.cpp \
                             eh_engine.cpp \
                             eh_stack_walker.cpp \
                             eh_execute_handler.arm.asm \
                             eh_invoke_funclet.arm.asm

module.dir.include.arm    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm
module.dir.sources.arm    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm



###############################
###    eh stuff arm64
###############################
module.sources.arm64      += eh_msvc_entry_points.cpp \
                             eh_engine.cpp \
                             eh_stack_walker.cpp \
                             eh_execute_handler.arm64.asm \
                             eh_invoke_funclet.arm64.asm \
                             eh_restore_context.arm64.asm

module.dir.include.arm    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm64
module.dir.sources.arm    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm64


ifndef module.path.base
  $(error $(module.name) module.path.base is not defined)
endif


#############################################################################
###    eh stuff
#############################################################################
module.sources            += eh_aux.cpp \
                             eh_msvc_internal_data_aux.cpp \
                             eh_msvc_array_support.cpp

module.path.include       += $(module.path.base)/eh
module.path.sources       += $(module.path.base)/eh



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
module.path.include.x86   += $(module.path.base)/eh/frame_based  $(module.path.base)/eh/frame_based/_x86
module.path.sources.x86   += $(module.path.base)/eh/frame_based  $(module.path.base)/eh/frame_based/_x86



###############################
###    eh stuff x64
###############################
module.sources.x64        += eh_msvc_entry_points.cpp \
                             eh_engine.cpp \
                             eh_stack_walker.cpp \
                             eh_execute_handler.x64.asm \
                             eh_restore_context.x64.asm \
                             eh_invoke_funclet.x64.asm

module.path.include.x64   += $(module.path.base)/eh/table_based  $(module.path.base)/eh/table_based/_x64
module.path.sources.x64   += $(module.path.base)/eh/table_based  $(module.path.base)/eh/table_based/_x64



###############################
###    eh stuff arm
###############################
module.sources.arm        += eh_msvc_entry_points.cpp \
                             eh_engine.cpp \
                             eh_stack_walker.cpp \
                             eh_invoke_funclet.arm.asm \
                             eh_execute_handler.arm.asm

module.path.include.arm   += $(module.path.base)/eh/table_based  $(module.path.base)/eh/table_based/_arm
module.path.sources.arm   += $(module.path.base)/eh/table_based  $(module.path.base)/eh/table_based/_arm



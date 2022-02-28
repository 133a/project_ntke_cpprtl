ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


#############################################################################
###    eh stuff
#############################################################################
module.sources            += eh_aux.cpp \
                             eh_array_support.cpp

module.dir.include        += $(module.dir.base)/eh
module.dir.sources        += $(module.dir.base)/eh



###############################
###    eh stuff x86
###############################
module.sources.x86        += eh_engine.cpp \
                             eh_stack_walk.cpp \
                             eh_funclet.cpp \
                             eh_seh_reg.cpp \
                             eh_safeseh.x86.asm

ifdef toolset.not_supported.thiscall
  module.sources.x86      += eh_array_support_thunk.x86.asm
endif
module.dir.include.x86    += $(module.dir.base)/eh/frame_based  $(module.dir.base)/eh/frame_based/_x86
module.dir.sources.x86    += $(module.dir.base)/eh/frame_based  $(module.dir.base)/eh/frame_based/_x86



###############################
###    eh stuff x64
###############################
module.sources.x64        += eh_engine.cpp \
                             eh_stack_walk.cpp \
                             eh_funclet.x64.asm \
                             eh_context.x64.asm \
                             eh_ep.x64.asm

module.dir.include.x64    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_x64
module.dir.sources.x64    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_x64



###############################
###    eh stuff arm
###############################
module.sources.arm        += eh_engine.cpp \
                             eh_stack_walk.cpp \
                             eh_funclet.arm.asm \
                             eh_context.arm.asm

module.dir.include.arm    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm
module.dir.sources.arm    += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm



###############################
###    eh stuff arm64
###############################
module.sources.arm64      += eh_engine.cpp \
                             eh_stack_walk.cpp \
                             eh_funclet.arm64.asm \
                             eh_context.arm64.asm

module.dir.include.arm64  += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm64
module.dir.sources.arm64  += $(module.dir.base)/eh/table_based  $(module.dir.base)/eh/table_based/_arm64


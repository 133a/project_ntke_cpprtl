module.sources         = eh_msvc_entry_points.cpp \
                         eh_msvc_array_support.cpp \
                         eh_msvc_internal_data.cpp \
                         eh_msvc_internal_data_aux.cpp \
                         eh_aux.cpp \
                         eh_engine.cpp \

module.sources.x86    += eh_exception_registration.cpp \
                         eh_stack_walker.cpp \
                         eh_except_list.x86.asm

ifdef toolset.not_supported.thiscall
  module.sources.x86  += eh_msvc_array_support_thunk.x86.asm
endif


ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif

module.dir.include = $(module.dir.base)  $(module.dir.base)/frame_based  $(module.dir.base)/frame_based/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.dir.sources = $(module.dir.base)  $(module.dir.base)/frame_based  $(module.dir.base)/frame_based/_$(target.arch)


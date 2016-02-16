module.sources = eh_msvc_entry_points.cpp \
                 eh_msvc_array_support.cpp \
                 eh_msvc_internal_data.cpp \
                 eh_msvc_internal_data_aux.cpp \
                 eh_aux.cpp \
                 eh_engine.cpp \
                 eh_exception_registration.cpp \
                 eh_stack_walker.cpp \
                 eh_except_list.x86.asm

ifdef toolset.not_supported.thiscall
  module.sources.x86 += eh_msvc_array_support_thunk.x86.asm
endif


ifndef module.path.base
$(error $(module.name) module.path.base is not defined)
endif

module.path.include = $(module.path.base)  $(module.path.base)/frame_based  $(module.path.base)/frame_based/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.path.sources = $(module.path.base)  $(module.path.base)/frame_based  $(module.path.base)/frame_based/_$(target.arch)


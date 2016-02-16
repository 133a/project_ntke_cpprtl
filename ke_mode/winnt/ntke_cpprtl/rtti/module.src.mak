module.sources = rtti_msvc_entry_points.cpp rtti_msvc_internal_data_aux.cpp rtti_engine.cpp rtti_type_info.cpp rtti_aux.cpp

ifndef module.path.base
$(error $(module.name) module.path.base is not defined)
endif

module.path.include  = $(module.path.base) $(module.path.base)/_$(target.arch)  $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.path.sources  = $(module.path.base) $(module.path.base)/_$(target.arch)


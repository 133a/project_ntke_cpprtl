module.sources = rtti_msvc_entry_points.cpp \
                 rtti_msvc_internal_data_aux.cpp \
                 rtti_engine.cpp \
                 rtti_type_info.cpp \
                 rtti_aux.cpp


ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


module.dir.include  = $(module.dir.base) $(module.dir.base)/_$(target.arch) $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.dir.sources  = $(module.dir.base) $(module.dir.base)/_$(target.arch)


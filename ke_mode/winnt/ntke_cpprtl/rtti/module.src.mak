module.sources = rtti_engine.cpp \
                 rtti_type_info.cpp \
                 rtti_aux.cpp \
                 rtti_get_co_dsc.$(target.arch).asm


ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


module.dir.include  = $(module.dir.base) $(module.dir.base)/_$(target.arch) $(project.dir.root)/ke_mode/winnt/ntke_cpprtl
module.dir.sources  = $(module.dir.base) $(module.dir.base)/_$(target.arch)

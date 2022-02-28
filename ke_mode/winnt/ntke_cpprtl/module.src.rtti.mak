ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif

#############################################################################
###    rtti stuff
#############################################################################
module.sources += rtti_engine.cpp \
                  rtti_type_info.cpp \
                  rtti_aux.cpp \
                  rtti_get_co_dsc.$(target.arch).asm

module.dir.include  += $(module.dir.base)/rtti  $(module.dir.base)/rtti/_$(target.arch)
module.dir.sources  += $(module.dir.base)/rtti  $(module.dir.base)/rtti/_$(target.arch)

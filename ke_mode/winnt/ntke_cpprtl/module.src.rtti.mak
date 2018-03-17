ifndef module.dir.base
  $(error define module.dir.base for $(module.name))
endif


#############################################################################
###    rtti stuff
#############################################################################
module.sources            += rtti_msvc_entry_points.cpp \
                             rtti_msvc_internal_data_aux.cpp \
                             rtti_engine.cpp \
                             rtti_type_info.cpp \
                             rtti_aux.cpp

module.dir.include        += $(module.dir.base)/rtti  $(module.dir.base)/rtti/_$(target.arch)
module.dir.sources        += $(module.dir.base)/rtti  $(module.dir.base)/rtti/_$(target.arch)


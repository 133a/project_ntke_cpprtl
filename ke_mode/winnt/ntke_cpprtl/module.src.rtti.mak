ifndef module.path.base
  $(error $(module.name) module.path.base is not defined)
endif


#############################################################################
###    rtti stuff
#############################################################################
module.sources            += rtti_msvc_entry_points.cpp \
                             rtti_msvc_internal_data_aux.cpp \
                             rtti_engine.cpp \
                             rtti_type_info.cpp \
                             rtti_aux.cpp

module.path.include       += $(module.path.base)/rtti  $(module.path.base)/rtti/_$(target.arch)
module.path.sources       += $(module.path.base)/rtti  $(module.path.base)/rtti/_$(target.arch)



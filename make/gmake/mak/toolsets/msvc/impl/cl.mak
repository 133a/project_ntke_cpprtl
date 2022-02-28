######################################################################################################
######################################################################################################
###
###    compiling c/c++ sources
###
######################################################################################################
######################################################################################################


ifndef toolset.type
  $(error define toolset.type)
endif
ifndef $(toolset.type).dir.toolset
  $(error define $(toolset.type).dir.toolset)
endif


###############################################
#      compiler enumerator
###############################################

  compiler.enum = $(target.kernel) \
                  $(target.arch) \
                  $(target.kernel)-$(target.arch) \
                  $(target.profile) \
                  $(target.profile).$(target.kernel) \
                  $(target.profile).$(target.arch) \
                  $(target.profile).$(target.kernel)-$(target.arch)



###############################################
#      module source files' search paths
###############################################

  vpath %.cpp  $(call enum2string_with_base_filtered,$(target.enum),module.dir.sources,make.filter.forward,:)
  vpath %.cxx  $(call enum2string_with_base_filtered,$(target.enum),module.dir.sources,make.filter.forward,:)
  vpath %.c    $(call enum2string_with_base_filtered,$(target.enum),module.dir.sources,make.filter.forward,:)



###############################################
#      compiler.definitions
###############################################

# sdk definitions
  compiler.definitions += $(call enum2list_with_base,$(sdk.enum),sdk.definitions,,)

# toolset-specific
  compiler.definitions += $(call enum2list_with_base,$(compiler.enum),toolset.definitions,,)

# aux toolset-specific
  compiler.definitions += $(call enum2list_with_base,$(compiler.enum),toolset.aux.definitions,,)

# target.kernel & target.arch & target.profile definitions
  compiler.definitions += $(call enum2list_with_base,$(compiler.enum),$(toolset.type).compiler.definitions,,)

# module definitions
  compiler.definitions += $(call enum2list_with_base,$(target.enum),module.definitions,,)



###############################################
#      compiler.includes
###############################################

# module includes
  compiler.includes += $(call enum2list_with_base,$(target.enum),module.dir.include,-I",")

# project includes
  compiler.includes += $(call enum2list,$(toolset.enum),project.dir.include,-I",")

# toolset & sdk includes
  compiler.includes += $(call enum2list_spaced,$(toolset.enum),toolset.aux.compiler.includes.head,-I",")
  compiler.includes += $(call enum2list_spaced,$(toolset.enum),sdk.dir.include,$(if $(toolset.sysinclude),$(toolset.sysinclude),-I)",")
  compiler.includes += $(call enum2list_spaced,$(toolset.enum),toolset.aux.compiler.includes.tail,-I",")



###############################################
#      compiler.options.cpp
###############################################

  compiler.options.cpp  = -TP
  compiler.options.cpp += $(call enum2list_with_base,$(compiler.enum),$(toolset.type).compiler.options,,)
  compiler.options.cpp += $(call enum2list_with_base,$(compiler.enum),$(toolset.type).compiler.options.cpp,,)

  compiler.options.cpp += $(call enum2list_with_base,,compiler.definitions,-D,)

  compiler.options.cpp += $(call enum2list_with_base,$(compiler.enum),toolset.aux.compiler.options,,)
  compiler.options.cpp += $(call enum2list_with_base,$(compiler.enum),toolset.aux.compiler.options.cpp,,)



###############################################
#      compiler.options.c
###############################################

  compiler.options.c  = -TC
  compiler.options.c += $(call enum2list_with_base,$(compiler.enum),$(toolset.type).compiler.options,,)
  compiler.options.c += $(call enum2list_with_base,$(compiler.enum),$(toolset.type).compiler.options.c,,)

  compiler.options.c += $(call enum2list_with_base,,compiler.definitions,-D,)

  compiler.options.c += $(call enum2list_with_base,$(compiler.enum),toolset.aux.compiler.options,,)
  compiler.options.c += $(call enum2list_with_base,$(compiler.enum),toolset.aux.compiler.options.c,,)



###############################################
#      compiler.cmdline
###############################################

  compiler.cmdline.output  = -Fo$@
  compiler.cmdline.output += -Fd$(@:%$(toolset.ext.obj)=%$(toolset.ext.pdb))
ifndef toolset.no_df
  compiler.cmdline.output += -showIncludes
endif

  compiler.cmdline.input   = -c $<



###############################################
#      .cpp .cxx template rules
###############################################

ifndef toolset.no_df
  compiler.cmdline.cpp  = "$($(toolset.type).df)"
endif
  compiler.cmdline.cpp += "$($(toolset.type).compiler)" $(compiler.options.cpp) $(compiler.includes) $(compiler.cmdline.output) $(compiler.cmdline.input)


$(target.dir.obj)/%$(toolset.ext.obj) : %.cpp
	$(compiler.cmdline.cpp)


$(target.dir.obj)/%$(toolset.ext.obj) : %.cxx
	$(compiler.cmdline.cpp)



###############################################
#      .c template rule
###############################################

ifndef toolset.no_df
  compiler.cmdline.c  = "$($(toolset.type).df)"
endif
  compiler.cmdline.c += "$($(toolset.type).compiler)" $(compiler.options.c) $(compiler.includes) $(compiler.cmdline.output) $(compiler.cmdline.input)


$(target.dir.obj)/%$(toolset.ext.obj) : %.c
	$(compiler.cmdline.c)



###############################################
#      generated header dependencies
###############################################

include $(wildcard $(target.dir.obj)/*.d)


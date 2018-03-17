######################################################################################################
######################################################################################################
###
###    translating arm sources
###
######################################################################################################
######################################################################################################


  armasm.enum = $(target.profile)


###############################################
#      composing source files search paths
###############################################

  vpath %.asm $(call enum2string_with_base_filtered,$(target.enum),module.dir.sources,make.filter.forward,:)



###############################################
#      armasm options
###############################################

  armasm.options += -nologo
  armasm.options += -coff_thumb2_only
  armasm.options += -g



###############################################
#      composing armasm options
###############################################

  armasm.options += $(call enum2list_with_base,$(armasm.enum),$(toolset.type).armasm.options,,)



###############################################
#      armasm includes
###############################################

# sdk includes
  armasm.includes += $(call enum2list_spaced,$(toolset.enum),sdk.dir.include,-I ",")



###############################################
#      C preprocessor
###############################################

  cpreprocessor = $($(toolset.type).compiler)



###############################################
#      C preprocessor definitions
###############################################

  cpreprocessor.definitions += $(call enum2list_with_base,$(armasm.enum),toolset.definitions,,)
  cpreprocessor.definitions += $(call enum2list_with_base,$(armasm.enum),toolset.aux.definitions,,)
  cpreprocessor.definitions += $(call enum2list_with_base,$(armasm.enum),$(toolset.type).armasm.definitions,,)
  cpreprocessor.definitions += $(call enum2list_with_base,$(target.enum),module.definitions,,)



###############################################
#      C preprocessor options
###############################################

  cpreprocessor.options  += -nologo
  cpreprocessor.options  += -E
  cpreprocessor.options  += $(call enum2list_with_base,,cpreprocessor.definitions,-D,)



###############################################
#      C preprocessor includes
###############################################

  cpreprocessor.includes += $(call enum2list_spaced,$(toolset.enum),sdk.dir.include,-I",")



###############################################
#      .arm.asm template rule
###############################################

$(target.dir.obj)/%$(toolset.ext.obj) : %.asm
	"$(cpreprocessor)" $(cpreprocessor.options) $(cpreprocessor.includes) $< > $(target.dir.obj)/$(notdir $<)
	"$($(toolset.type).armasm)" $(armasm.includes) $(armasm.options) -o $@ $(target.dir.obj)/$(notdir $<)


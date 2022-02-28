######################################################################################################
######################################################################################################
###
###    translating x86/x64 sources
###
######################################################################################################
######################################################################################################


  assembler.enum = $(target.arch) \
                   $(target.profile) \
                   $(target.profile).$(target.arch)


###############################################
#      composing source files search paths
###############################################

  vpath %.asm $(call enum2string_with_base_filtered,$(target.enum),module.dir.sources,make.filter.forward,:)



###############################################
#      common masm options
###############################################

  assembler.options += -nologo
  assembler.options += -X
  assembler.options += -Cx
  assembler.options += -W3
  assembler.options += -Zd
  assembler.options += -Zi
ifndef toolset.not_supported.safeseh
  ifneq '$(target.kernel)' 'nt5.0'
    assembler.options.x86 += -safeseh
  endif
endif



###############################################
#      composing masm options
###############################################

  assembler.options += $(call enum2list,$(assembler.enum),assembler.options,,)
  assembler.options += $(call enum2list_with_base,$(assembler.enum),$(toolset.type).assembler.options,,)
                            


###############################################
#      masm includes
###############################################

# sdk includes
  assembler.includes += $(call enum2list_spaced,$(toolset.enum),sdk.dir.include,-I",")



###############################################
#      .x86.asm .x64.asm template rule
###############################################

$(target.dir.obj)/%$(toolset.ext.obj) : %.asm
	"$($(toolset.type).assembler)" $(assembler.includes) $(assembler.options) -Fo$@ -c $<


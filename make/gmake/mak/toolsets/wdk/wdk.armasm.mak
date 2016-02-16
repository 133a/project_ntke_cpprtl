###############################################
#      composing source files search paths
###############################################

  module.path.sources ?= ./
  vpath %.asm $(call enum2list_with_base,$(wdk.module.enum),module.path.sources,,:)



###############################################
#      armasm options
###############################################

  wdk.assembler.options += -nologo -coff_thumb2_only


# $(target.profile) options
  wdk.assembler.options.debug.common += -g
  wdk.assembler.options.release.common += 

  wdk.assembler.options.debug      = $(wdk.assembler.options.debug.common)
  wdk.assembler.options.release    = $(wdk.assembler.options.release.common)
  wdk.assembler.options.release_Os = $(wdk.assembler.options.release.common)
  wdk.assembler.options.release_Ot = $(wdk.assembler.options.release.common)
  wdk.assembler.options.release_Ox = $(wdk.assembler.options.release.common)


# composing the assembler options
  wdk.assembler.options += $(wdk.assembler.options.$(target.profile))


# composing the assembler includes
  wdk.assembler.includes += $(call enum2list_spaced,$(make.enum),wdk.dir.include,-I ",")



###############################################
#      C preprocessor
###############################################

# preprocessing definitions
  wdk.cpreprocessor.definitions += NT_KERNEL_MODE

ifdef module.definitions
  wdk.cpreprocessor.definitions += $(module.definitions)
endif

ifdef toolset.definitions
  wdk.cpreprocessor.definitions += $(toolset.definitions)
endif

ifdef wdk.aux.compiler.definitions
  wdk.cpreprocessor.definitions += $(wdk.aux.compiler.definitions)
endif



  wdk.cpreprocessor = $(wdk.compiler)

  wdk.cpreprocessor.options  += -nologo -E
  wdk.cpreprocessor.includes += $(call enum2list_spaced,$(make.enum),wdk.dir.include,-I",")

  wdk.cpreprocessor.options  += $(call enum2list_with_base,,wdk.cpreprocessor.definitions,-D,)




###############################################
#      the template rule
###############################################

$(target.dir.obj)/%.obj : %.asm
	"$(wdk.cpreprocessor)" $(wdk.cpreprocessor.options) $(wdk.cpreprocessor.includes) $< > $(target.dir.obj)/$(notdir $<)
	"$(wdk.assembler)" $(wdk.assembler.includes) $(wdk.assembler.options) -o $@ $(target.dir.obj)/$(notdir $<)


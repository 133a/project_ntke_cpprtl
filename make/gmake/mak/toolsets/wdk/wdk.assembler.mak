###############################################
###    choosing the target-specific assembler
###############################################

ifeq '$(target.arch)' 'ia64'
  include $(make.dir.toolsets)/wdk/wdk.ias.mak
else ifeq '$(target.arch)' 'arm'
  include $(make.dir.toolsets)/wdk/wdk.armasm.mak
else # x86/x64-asm processing



###############################################
#      composing source files search paths
###############################################

  module.path.sources ?= ./
  vpath %.asm $(call enum2list_with_base,$(wdk.module.enum),module.path.sources,,:)



###############################################
#      assembler options
###############################################

  wdk.assembler.options += -c -nologo -X -DNT_KERNEL_MODE -DDDK_VER=$(wdk.version)


# $(target.profile) options
  wdk.assembler.options.debug.common += -Zd -Zi
  wdk.assembler.options.release.common += 

  wdk.assembler.options.debug      = $(wdk.assembler.options.debug.common)
  wdk.assembler.options.release    = $(wdk.assembler.options.release.common)
  wdk.assembler.options.release_Os = $(wdk.assembler.options.release.common)
  wdk.assembler.options.release_Ot = $(wdk.assembler.options.release.common)
  wdk.assembler.options.release_Ox = $(wdk.assembler.options.release.common)


# composing the assembler options
  wdk.assembler.options += $(wdk.assembler.options.$(target.profile))


# composing the assembler includes
  wdk.assembler.includes += $(call enum2list_spaced,$(make.enum),wdk.dir.include,-I",")



###############################################
#      the template rule
###############################################

$(target.dir.obj)/%.obj : %.asm
	"$(wdk.assembler)" -Fo$@ $(wdk.assembler.includes) $(wdk.assembler.options) $<


endif # x86/x64-asm processing


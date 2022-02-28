######################################################################################################
######################################################################################################
###
###    linking a binary
###
######################################################################################################
######################################################################################################


###############################################
#    linker enumerator
###############################################

  linker.enum   = $(target.kernel) \
                  $(target.arch) \
                  $(target.kernel)-$(target.arch) \
                  $(target.profile) \
                  $(target.profile).$(target.kernel) \
                  $(target.profile).$(target.arch) \
                  $(target.profile).$(target.kernel)-$(target.arch)



###############################################
#    composing the linker options
###############################################

  linker.options += $(call enum2list_with_base,$(linker.enum),$(toolset.type).linker.options,,)
  linker.options += $(call enum2list_with_base,$(linker.enum),toolset.aux.linker.options,,)



###############################################
#    linker libpath
###############################################

  linker.libpath += -LIBPATH:"$(target.dir.obj)"
  linker.libpath += $(call enum2list_spaced,$(toolset.enum),toolset.aux.linker.libpath.head,-LIBPATH:",")
  linker.libpath += $(call enum2list_spaced,$(toolset.enum),sdk.dir.lib,-LIBPATH:",")
  linker.libpath += $(call enum2list_spaced,$(toolset.enum),toolset.aux.linker.libpath.tail,-LIBPATH:",")



###############################################
#    linker lib list
###############################################

ifeq '$(module.subsystem)'  'nt_native'
  target.lib.sdk  = ntdll.lib
else
  target.lib.sdk += $(addsuffix $(toolset.ext.lib),$(call enum2list_with_base,$(target.enum),module.lib.sdk,,))
endif



###############################################
#    building the necessary projects' libraries
###############################################

  target.lib.depend = $(call enum2list_with_base,$(target.enum),module.lib.depend,,)

.PHONY : $(target.lib.depend)
$(target.lib.depend) :
	"$(MAKE)" --directory=$(if $(filter .%,$@),,$(project.dir.root)/)$(dir $@) -f $(make.makefile)



###############################################
#    building the main target
###############################################

  target.libs = \
    $(foreach i,\
      $(addsuffix $(toolset.ext.lib),$(notdir $(target.lib.depend))),\
      $(project.dir.output)/$i/$(target.toolset)/$(target.dir.spec)/$i\
     )


$(target.file) : $(target.libs) $(target.objects)
	"$($(toolset.type).linker)" $(linker.options) $(linker.libpath) -OUT:"$@" $(target.lib.sdk) $(target.libs) $(notdir $(target.objects))


  target.filename = $(target.dir.build)/$(target.name)$(target.ext)
  target.main.depends += $(target.lib.depend) $(target.filename)



# common linker options
# -SECTION:INIT,d -FORCE:MULTIPLE
  wdk.linker.options += -NOLOGO -SUBSYSTEM:NATIVE -INCREMENTAL:NO -NODEFAULTLIB -BASE:0x10000 -OPT:REF -OPT:ICF -LIBPATH:"$(target.dir.obj)"

ifdef module.drivertype.wdm
  wdk.linker.options += -DRIVER:WDM
else
  wdk.linker.options += -DRIVER
endif

ifndef module.entry_point
  # wdk.linker.options += -ENTRY:DriverEntry
  $(error module.entry_point is not defined)
endif
  # x86 decor for a function taking 2 ptr args
  wdk.linker.entry_point_tail.x86 = @8
  wdk.linker.options += -ENTRY:$(module.entry_point)$(wdk.linker.entry_point_tail.$(target.arch))



# $(target.arch) options
  wdk.linker.options.x86 += -MACHINE:X86
ifndef toolset.not_supported.safeseh
  wdk.linker.options.x86 += -SAFESEH:NO
endif
  wdk.linker.options.x64  += -MACHINE:$(if $(wdk.linker.option.machine),$(wdk.linker.option.machine),X64)
  wdk.linker.options.ia64 += -MACHINE:IA64
  wdk.linker.options.arm  += -MACHINE:ARM



# $(target.profile) options
  wdk.linker.options.debug.common   += -DEBUG
  wdk.linker.options.release.common += -DEBUG -RELEASE -INCREMENTAL:NO

  wdk.linker.options.debug      = $(wdk.linker.options.debug.common)
  wdk.linker.options.release    = $(wdk.linker.options.release.common)
  wdk.linker.options.release_Os = $(wdk.linker.options.release.common)
  wdk.linker.options.release_Ot = $(wdk.linker.options.release.common)
  wdk.linker.options.release_Ox = $(wdk.linker.options.release.common)



# $(target.kernel) options
  wdk.linker.options.nt5.0  +=
  wdk.linker.options.nt5.1  +=
  wdk.linker.options.nt5.2  +=
  wdk.linker.options.nt6.0  +=
  wdk.linker.options.nt6.1  +=
  wdk.linker.options.nt6.2  +=
  wdk.linker.options.nt6.3  +=
  wdk.linker.options.nt10.0 +=



# composing the linker options
  wdk.linker.options += $(wdk.linker.options.$(target.arch)) $(wdk.linker.options.$(target.profile)) $(wdk.linker.options.$(target.kernel))
  wdk.linker.libpath = $(call enum2list_spaced,$(make.enum),wdk.dir.lib,-LIBPATH:",")
  target.lib.sdk += $(call enum2list_with_base,$(wdk.module.enum),module.lib.sdk,,)


### building the necessary projects' libraries

  target.lib.depend = $(call enum2list_with_base,$(wdk.module.enum),module.lib.depend,,)
.PHONY : $(target.lib.depend)


$(target.lib.depend) :
	"$(make.make)" --directory=$(if $(filter .%,$@),,$(project.dir.root)/)$(dir $@) -f $(make.makefile) $(target.make.forward)




### building the main target

target.lib = $(foreach i,$(addsuffix .lib,$(notdir $(target.lib.depend))), $(project.dir.output)/$i/$(target.toolset)/$(target.profile)_$(target.arch)_$(target.kernel)/$i)


$(target.filename) : $(target.lib) $(target.objects)
	"$(wdk.linker)" $(wdk.linker.options) $(wdk.linker.libpath) -OUT:"$@" $(target.lib.sdk) $(target.lib) $(notdir $(target.objects))


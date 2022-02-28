###############################################
#    common linker options
###############################################

# -SECTION:INIT,d -FORCE:MULTIPLE
  wdk.linker.options += -NOLOGO
  wdk.linker.options += -DRIVER
  wdk.linker.options += -SUBSYSTEM:NATIVE
  wdk.linker.options += -INCREMENTAL:NO
  wdk.linker.options += -NODEFAULTLIB
  wdk.linker.options += -OPT:REF
  wdk.linker.options += -OPT:ICF


# target.arch options
  wdk.linker.options.x86    += -MACHINE:X86
  wdk.linker.options.x64    += -MACHINE:$(if $(toolset.option.machine),$(toolset.option.machine),X64)
  wdk.linker.options.arm    += -MACHINE:ARM
  wdk.linker.options.arm64  += -MACHINE:ARM64
  wdk.linker.options.ia64   += -MACHINE:IA64
ifndef toolset.not_supported.safeseh
  ifneq '$(target.kernel)' 'nt5.0'
    wdk.linker.options.x86  += -SAFESEH
  else
    wdk.linker.options.x86  += -SAFESEH:NO
  endif
endif


ifndef module.base.$(target.arch)
    module.base.$(target.arch) = 0x100000
  ifdef toolset.supported.force_aslr
    module.base.x64            = 0x110000000
    module.base.arm64          = 0x110000000
  endif
endif
  wdk.linker.options += -BASE:$(module.base.$(target.arch))


###############################################
#    module-specific linker options
###############################################

# entry point
ifndef module.entry_point
  $(error define module.entry_point)
endif
  # x86 decor for the __stdcall function taking 2 ptr args
  wdk.linker.entry_point_tail.x86 = @8
  wdk.linker.options += -ENTRY:$(module.entry_point)$(wdk.linker.entry_point_tail.$(target.arch))


###############################################
#    target.profile linker options
###############################################

# debug
  wdk.linker.options.debug           += -DEBUG


# release
  wdk.linker.options.release         += -DEBUG -RELEASE


# cpprtl_debug
  wdk.linker.options.cpprtl_debug    += -DEBUG


# cpprtl_release
  wdk.linker.options.cpprtl_release  += -DEBUG -RELEASE


###############################################
#      include the linker engine
###############################################

include $(msvc.dir.toolset)/impl/link.mak

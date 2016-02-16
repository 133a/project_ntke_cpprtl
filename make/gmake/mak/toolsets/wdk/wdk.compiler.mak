###############################################
#      module source files' search paths
###############################################

  module.path.sources ?= ./
  vpath %.cpp $(call enum2list_with_base,$(wdk.module.enum),module.path.sources,,:)
  vpath %.cxx $(call enum2list_with_base,$(wdk.module.enum),module.path.sources,,:)
  vpath %.c   $(call enum2list_with_base,$(wdk.module.enum),module.path.sources,,:)



###############################################
#      wdk.compiler.definitions
###############################################


  wdk.options.enum = $(target.profile) $(target.arch) $(target.kernel)


  wdk.compiler.definitions += NT_KERNEL_MODE DDK_VER=$(wdk.version)

ifdef module.drivertype.wdm
  wdk.compiler.definitions += DRIVER_TYPE_WDM
endif


# $(target.profile options)
  wdk.compiler.definitions.debug.common   += DBG _DEBUG
  wdk.compiler.definitions.release.common += NDEBUG

  wdk.compiler.definitions.debug      += $(wdk.compiler.definitions.debug.common)
  wdk.compiler.definitions.release    += $(wdk.compiler.definitions.release.common)
  wdk.compiler.definitions.release_Os += $(wdk.compiler.definitions.release.common)
  wdk.compiler.definitions.release_Ot += $(wdk.compiler.definitions.release.common)
  wdk.compiler.definitions.release_Ox += $(wdk.compiler.definitions.release.common)


# $(target.arch) options, _WIN32 is implicitly predefined, _WIN64 is implicitly predefined for 64bit
  wdk.compiler.definitions.x86  += _X86_ i386 WIN32 
  wdk.compiler.definitions.x64  += _AMD64_ AMD64 WIN32 WIN64
  wdk.compiler.definitions.ia64 += _IA64_ WIN32 WIN64
  wdk.compiler.definitions.arm  += _ARM_ WIN32 _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE


# $(target.kernel) options
  wdk.compiler.definitions.nt5.0  += _WIN32_WINNT=0x0500 WINVER=0x0500 NTDDI_VERSION=0x05000000
  wdk.compiler.definitions.nt5.1  += _WIN32_WINNT=0x0501 WINVER=0x0501 NTDDI_VERSION=0x05010000
  wdk.compiler.definitions.nt5.2  += _WIN32_WINNT=0x0502 WINVER=0x0502 NTDDI_VERSION=0x05020000
  wdk.compiler.definitions.nt6.0  += _WIN32_WINNT=0x0600 WINVER=0x0600 NTDDI_VERSION=0x06000000
  wdk.compiler.definitions.nt6.1  += _WIN32_WINNT=0x0601 WINVER=0x0601 NTDDI_VERSION=0x06010000
  wdk.compiler.definitions.nt6.2  += _WIN32_WINNT=0x0602 WINVER=0x0602 NTDDI_VERSION=0x06020000
  wdk.compiler.definitions.nt6.3  += _WIN32_WINNT=0x0603 WINVER=0x0603 NTDDI_VERSION=0x06030000
  wdk.compiler.definitions.nt10.0 += _WIN32_WINNT=0x0A00 WINVER=0x0A00 NTDDI_VERSION=0x0A000000



ifdef module.definitions
  wdk.compiler.definitions += $(module.definitions)
endif

ifdef toolset.definitions
  wdk.compiler.definitions += $(toolset.definitions)
endif

ifdef wdk.aux.compiler.definitions
  wdk.compiler.definitions += $(wdk.aux.compiler.definitions)
endif




###############################################
#      wdk.compiler.optimize
###############################################

  wdk.compiler.optimize.debug    = -Od
ifdef  wdk.aux.compiler.optimize.debug  #  override
  wdk.compiler.optimize.debug    = $(wdk.aux.compiler.optimize.debug)
endif

  wdk.compiler.optimize.release  = -Ox
ifdef  wdk.aux.compiler.optimize.release  #  override
  wdk.compiler.optimize.release  = $(wdk.aux.compiler.optimize.release)
endif





###############################################
#      wdk.compiler.options.cpp
###############################################


  wdk.compiler.options.cpp += -c -nologo -TP -W3 -J -Zc:forScope -GF -Zp8

ifdef toolset.supported.thread_safe_static
  wdk.compiler.options.cpp += -Zc:threadSafeInit-
endif


# turn off -GS and -RTC if those are supported by compiler 
ifndef toolset.not_supported.RTC_and_GS
  wdk.compiler.options.cpp += -GS- 
endif


# exception handling compiler option
ifdef module.EHcpp
  ifeq      '$(module.EHcpp)'  'a'
    wdk.compiler.options.cpp += -EHa
  else ifeq '$(module.EHcpp)'  's'
    wdk.compiler.options.cpp += -EHs
  endif
endif


# RTTI compiler option
ifdef module.RTTI
  wdk.compiler.options.cpp += -GR
else
  wdk.compiler.options.cpp += -GR-
endif


# $(target.profile options)
  wdk.compiler.options.cpp.debug.common    += -Zi
  wdk.compiler.options.cpp.release.common  += -Zi

  wdk.compiler.options.cpp.debug      += $(wdk.compiler.options.cpp.debug.common) $(wdk.compiler.optimize.debug)
  wdk.compiler.options.cpp.release    += $(wdk.compiler.options.cpp.release.common) $(wdk.compiler.optimize.release)
  wdk.compiler.options.cpp.release_Os += $(wdk.compiler.options.cpp.release.common) -Os
  wdk.compiler.options.cpp.release_Ot += $(wdk.compiler.options.cpp.release.common) -Ot
  wdk.compiler.options.cpp.release_Ox += $(wdk.compiler.options.cpp.release.common) -Ox


# $(target.arch) options
  wdk.compiler.options.cpp.x86  += -Gz -Zl
  wdk.compiler.options.cpp.x64  +=
  wdk.compiler.options.cpp.ia64 +=
  wdk.compiler.options.cpp.arm  +=


# $(target.kernel) options
  wdk.compiler.options.cpp.nt5.0  +=
  wdk.compiler.options.cpp.nt5.1  +=
  wdk.compiler.options.cpp.nt5.2  +=
  wdk.compiler.options.cpp.nt6.0  +=
  wdk.compiler.options.cpp.nt6.1  +=
  wdk.compiler.options.cpp.nt6.2  +=
  wdk.compiler.options.cpp.nt6.3  +=
  wdk.compiler.options.cpp.nt10.0 +=


# composing the compiler options
  wdk.compiler.options.cpp += $(call enum2list,$(wdk.options.enum),wdk.compiler.options.cpp,,)
  wdk.compiler.options.cpp += $(call enum2list_with_base,$(wdk.options.enum),wdk.compiler.definitions,-D,)


  wdk.compiler.options.cpp += $(wdk.aux.compiler.options.cpp)




###############################################
#      wdk.compiler.options.c
###############################################


# 
  wdk.compiler.options.c += -c -nologo -TC -W3 -J -Zc:forScope -GF -Zp8

ifdef toolset.supported.thread_safe_static
  wdk.compiler.options.c += -Zc:threadSafeInit-
endif


# turn off -GS and -RTC if those are supported by compiler 
ifndef toolset.not_supported.RTC_and_GS
  wdk.compiler.options.c += -GS- 
endif


# $(target.profile options)
  wdk.compiler.options.c.debug.common    += -Zi -Od
  wdk.compiler.options.c.release.common  += -Zi -Ox

  wdk.compiler.options.c.debug      += $(wdk.compiler.options.c.debug.common) $(wdk.compiler.optimize.debug)
  wdk.compiler.options.c.release    += $(wdk.compiler.options.c.release.common) $(wdk.compiler.optimize.release)
  wdk.compiler.options.c.release_Os += $(wdk.compiler.options.c.release.common) -Os
  wdk.compiler.options.c.release_Ot += $(wdk.compiler.options.c.release.common) -Ot
  wdk.compiler.options.c.release_Ox += $(wdk.compiler.options.c.release.common) -Ox


# $(target.arch) options
  wdk.compiler.options.c.x86  += -Gz -Zl
  wdk.compiler.options.c.x64  +=
  wdk.compiler.options.c.ia64 +=
  wdk.compiler.options.c.arm  +=


# $(target.kernel) options
  wdk.compiler.options.c.nt5.0  +=
  wdk.compiler.options.c.nt5.1  +=
  wdk.compiler.options.c.nt5.2  +=
  wdk.compiler.options.c.nt6.0  +=
  wdk.compiler.options.c.nt6.1  +=
  wdk.compiler.options.c.nt6.2  +=
  wdk.compiler.options.c.nt6.3  +=
  wdk.compiler.options.c.nt10.0 +=


# composing the compiler options
  wdk.compiler.options.c += $(call enum2list,$(wdk.options.enum),wdk.compiler.options.cpp,,)
  wdk.compiler.options.c += $(call enum2list_with_base,$(wdk.options.enum),wdk.compiler.definitions,-D,)


  wdk.compiler.options.c += $(wdk.aux.compiler.options.c)




###############################################
#      wdk.compiler.includes
###############################################


  wdk.compiler.includes += $(call enum2list_with_base,$(wdk.module.enum),module.path.include,-I",")
  wdk.compiler.includes += $(call enum2list,$(make.enum),project.dir.include,-I",")
  wdk.compiler.includes += $(call enum2list_spaced,$(make.enum),wdk.dir.include,-I",")





###############################################
#      template rules for .cpp .c
###############################################


# compiling C++2003 sources

ifndef make.no_df
  wdk.compiler.options.cpp += -showIncludes
  wdk.compiler.cmdline.cpp += "$(wdk.df)"
endif
  wdk.compiler.cmdline.cpp += "$(wdk.compiler)" $(wdk.compiler.options.cpp) $(wdk.compiler.includes) -Fo$@ -Fd$(@:.obj=.pdb) $<


$(target.dir.obj)/%.obj : %.cpp
	$(wdk.compiler.cmdline.cpp)



# compiling C sources

ifndef make.no_df
  wdk.compiler.options.c += -showIncludes
  wdk.compiler.cmdline.c += "$(wdk.df)"
endif
  wdk.compiler.cmdline.c += "$(wdk.compiler)" $(wdk.compiler.options.c) $(wdk.compiler.includes) -Fo$@ -Fd$(@:.obj=.pdb) $<


$(target.dir.obj)/%.obj : %.c
	$(wdk.compiler.cmdline.c)





###############################################
#      header dependencies generated by DF using compiler output induced by option -showIncludes
###############################################

include $(wildcard $(target.dir.obj)/*.d)


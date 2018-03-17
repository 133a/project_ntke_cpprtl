ifndef DDK2600_FOLDER
  $(error define DDK2600_FOLDER)
endif


ddk2600.dir            = $(subst \,/,$(DDK2600_FOLDER))


ddk2600.dir.bin        = $(ddk2600.dir)/bin
ddk2600.dir.include    = $(ddk2600.dir)/inc
ddk2600.dir.lib        = $(ddk2600.dir)/lib


# Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.00.9176 for 80x86
ddk2600.compiler       = $(ddk2600.dir.bin)/x86/cl.exe
wdk.compiler           = $(ddk2600.compiler)

ddk2600.assembler      = $(ddk2600.dir.bin)/x86/ml.exe
wdk.assembler          = $(ddk2600.assembler)

ddk2600.linker         = $(ddk2600.dir.bin)/x86/link.exe
wdk.linker             = $(ddk2600.linker)

ddk2600.libtool        = $(ddk2600.dir.bin)/x86/lib.exe
wdk.libtool            = $(ddk2600.libtool)


ifeq      '$(target.kernel)'  'nt5.0'
  ddk2600.dir.kernel = w2k
else ifeq '$(target.kernel)'  'nt5.1'
  ddk2600.dir.kernel = wxp
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk.dir.include.win32     = $(ddk2600.dir.include)/$(ddk2600.dir.kernel)
wdk.dir.include.crt       = $(ddk2600.dir.include)/crt
wdk.dir.include.ddk       = $(ddk2600.dir.include)/ddk/$(ddk2600.dir.kernel)
wdk.dir.include.wdm       = $(ddk2600.dir.include)/ddk/wdm/$(ddk2600.dir.kernel)
wdk.dir.lib.wdm           = $(ddk2600.dir.lib)/$(ddk2600.dir.kernel)/i386


sdk.dir.include.1         = $(wdk.dir.include.win32)
sdk.dir.include.2         = $(wdk.dir.include.crt)
sdk.dir.include.3         = $(wdk.dir.include.ddk)
sdk.dir.include.4         = $(wdk.dir.include.wdm)
sdk.dir.lib.1             = $(wdk.dir.lib.wdm)


wdk.df                    = $(make.dir.bin)/x86/df.exe


wdk.version = 2600
target.arch = x86
toolset.arch = x86
toolset.family = ddk2600
toolset.supported.kernels = nt5.0 nt5.1
toolset.not_supported.signing = 1
toolset.not_supported.RTC_and_GS = 1
toolset.not_supported.safeseh = 1
toolset.not_supported.thiscall = 1
toolset.not_supported.template_partial_specialization = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


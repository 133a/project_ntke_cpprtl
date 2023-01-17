# Windows 2003 SP1 DDK 3790.1830

ifndef DDK3790_FOLDER
  $(error define DDK3790_FOLDER)
endif


ddk3790.dir          = $(subst \,/,$(DDK3790_FOLDER))


ddk3790.dir.bin      = $(ddk3790.dir)/bin
ddk3790.dir.include  = $(ddk3790.dir)/inc
ddk3790.dir.lib      = $(ddk3790.dir)/lib


# Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.4035 for 80x86
ddk3790.compiler     = $(ddk3790.dir.bin)/x86/cl.exe
wdk.compiler         = $(ddk3790.compiler)

# Microsoft (R) Macro Assembler Version 7.10.4035
ddk3790.assembler    = $(ddk3790.dir.bin)/x86/ml.exe
wdk.assembler        = $(ddk3790.assembler)

# Microsoft (R) Incremental Linker Version 7.10.4035
ddk3790.linker       = $(ddk3790.dir.bin)/x86/link.exe
wdk.linker           = $(ddk3790.linker)

# Microsoft (R) Library Manager Version 7.10.4035
ddk3790.libmgr       = $(ddk3790.dir.bin)/x86/lib.exe
wdk.libmgr           = $(ddk3790.libmgr)


ddk3790.dir.nt5.0      = w2k
ddk3790.dir.nt5.1      = wxp
ddk3790.dir.nt5.2      = wnet


wdk.dir.include.win32  = $(ddk3790.dir.include)/$(ddk3790.dir.$(target.kernel))
wdk.dir.include.crt    = $(ddk3790.dir.include)/crt
wdk.dir.include.ddk    = $(ddk3790.dir.include)/ddk/$(ddk3790.dir.$(target.kernel))
wdk.dir.include.wdm    = $(ddk3790.dir.include)/ddk/wdm/$(ddk3790.dir.$(target.kernel))
wdk.dir.lib.wdm        = $(ddk3790.dir.lib)/$(ddk3790.dir.$(target.kernel))/i386


sdk.dir.include.1      = $(wdk.dir.include.win32)
sdk.dir.include.2      = $(wdk.dir.include.crt)
sdk.dir.include.3      = $(wdk.dir.include.ddk)
sdk.dir.include.4      = $(wdk.dir.include.wdm)
sdk.dir.lib.1          = $(wdk.dir.lib.wdm)


wdk.df = $(make.dir.bin)/x86/df.exe


wdk.version = 3790
target.arch = x86
toolset.arch = x86
toolset.family = ddk3790
toolset.supported.kernels = nt5.0 nt5.1 nt5.2
toolset.not_supported.signing = 1
toolset.not_supported.RTC_and_GS = 1
toolset.not_supported.thiscall = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


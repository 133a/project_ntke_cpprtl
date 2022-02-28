# Windows 2003 SP1 DDK 3790.1830

ifndef DDK3790_FOLDER
  $(error define DDK3790_FOLDER)
endif


ddk3790.dir          = $(subst \,/,$(DDK3790_FOLDER))


ddk3790.dir.bin      = $(ddk3790.dir)/bin
ddk3790.dir.include  = $(ddk3790.dir)/inc
ddk3790.dir.lib      = $(ddk3790.dir)/lib


# Microsoft (R) C/C++ Optimizing Compiler Version 14.00.40310.41 for AMD64
ddk3790.compiler     = $(ddk3790.dir.bin)/win64/x86/amd64/cl.exe
wdk.compiler         = $(ddk3790.compiler)

# Microsoft (R) Macro Assembler (AMD64) Version 8.00.40310.39
ddk3790.assembler    = $(ddk3790.dir.bin)/win64/x86/amd64/ml64.exe
wdk.assembler        = $(ddk3790.assembler)

# Microsoft (R) Incremental Linker Version 8.00.40310.39
ddk3790.linker       = $(ddk3790.dir.bin)/win64/x86/amd64/link.exe
wdk.linker           = $(ddk3790.linker)

# Microsoft (R) Library Manager Version 8.00.40310.39
ddk3790.libmgr       = $(ddk3790.dir.bin)/win64/x86/amd64/lib.exe
wdk.libmgr           = $(ddk3790.libmgr)


ifeq '$(target.kernel)' 'nt5.2'
  ddk3790.dir.kernel = wnet
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk.dir.include.win32  = $(ddk3790.dir.include)/$(ddk3790.dir.kernel)
wdk.dir.include.crt    = $(ddk3790.dir.include)/crt
wdk.dir.include.ddk    = $(ddk3790.dir.include)/ddk/$(ddk3790.dir.kernel)
wdk.dir.include.wdm    = $(ddk3790.dir.include)/ddk/wdm/$(ddk3790.dir.kernel)
wdk.dir.lib.wdm        = $(ddk3790.dir.lib)/$(ddk3790.dir.kernel)/amd64


sdk.dir.include.1      = $(wdk.dir.include.win32)
sdk.dir.include.2      = $(wdk.dir.include.crt)
sdk.dir.include.3      = $(wdk.dir.include.ddk)
sdk.dir.include.4      = $(wdk.dir.include.wdm)
sdk.dir.lib.1          = $(wdk.dir.lib.wdm)


wdk.df = $(make.dir.bin)/x86/df.exe


wdk.version = 3790
target.arch = x64
toolset.arch = x86
toolset.family = ddk3790
toolset.supported.kernels = nt5.2
toolset.not_supported.signing = 1
toolset.option.machine = AMD64


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


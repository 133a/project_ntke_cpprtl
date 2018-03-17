ifndef DDK3790SP1_FOLDER
  $(error define DDK3790SP1_FOLDER)
endif


ddk3790sp1.dir            = $(subst \,/,$(DDK3790SP1_FOLDER))


ddk3790sp1.dir.bin        = $(ddk3790sp1.dir)/bin
ddk3790sp1.dir.include    = $(ddk3790sp1.dir)/inc
ddk3790sp1.dir.lib        = $(ddk3790sp1.dir)/lib


# Microsoft (R) C/C++ Optimizing Compiler Version 14.00.40310.41 for AMD64
ddk3790sp1.compiler       = $(ddk3790sp1.dir.bin)/win64/x86/amd64/cl.exe
wdk.compiler              = $(ddk3790sp1.compiler)

ddk3790sp1.assembler      = $(ddk3790sp1.dir.bin)/win64/x86/amd64/ml64.exe
wdk.assembler             = $(ddk3790sp1.assembler)

ddk3790sp1.linker         = $(ddk3790sp1.dir.bin)/win64/x86/amd64/link.exe
wdk.linker                = $(ddk3790sp1.linker)

ddk3790sp1.libtool        = $(ddk3790sp1.dir.bin)/win64/x86/amd64/lib.exe
wdk.libtool               = $(ddk3790sp1.libtool)


ifeq '$(target.kernel)' 'nt5.2'
  ddk3790sp1.dir.kernel = wnet
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk.dir.include.win32     = $(ddk3790sp1.dir.include)/$(ddk3790sp1.dir.kernel)
wdk.dir.include.crt       = $(ddk3790sp1.dir.include)/crt
wdk.dir.include.ddk       = $(ddk3790sp1.dir.include)/ddk/$(ddk3790sp1.dir.kernel)
wdk.dir.include.wdm       = $(ddk3790sp1.dir.include)/ddk/wdm/$(ddk3790sp1.dir.kernel)
wdk.dir.lib.wdm           = $(ddk3790sp1.dir.lib)/$(ddk3790sp1.dir.kernel)/amd64


sdk.dir.include.1         = $(wdk.dir.include.win32)
sdk.dir.include.2         = $(wdk.dir.include.crt)
sdk.dir.include.3         = $(wdk.dir.include.ddk)
sdk.dir.include.4         = $(wdk.dir.include.wdm)
sdk.dir.lib.1             = $(wdk.dir.lib.wdm)


wdk.df                    = $(make.dir.bin)/x86/df.exe


wdk.version = 3790
target.arch = x64
toolset.arch = x86
toolset.family = ddk3790sp1
toolset.supported.kernels = nt5.2
toolset.not_supported.signing = 1
toolset.option.machine = AMD64


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


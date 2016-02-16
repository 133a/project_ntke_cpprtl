ifndef DDK3790SP1_FOLDER
  $(error DDK3790SP1_FOLDER is not defined)
endif


ddk3790sp1.dir            = $(subst \,/,$(DDK3790SP1_FOLDER))


ddk3790sp1.dir.bin        = $(ddk3790sp1.dir)/bin
ddk3790sp1.dir.include    = $(ddk3790sp1.dir)/inc
ddk3790sp1.dir.lib        = $(ddk3790sp1.dir)/lib


# Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.4035 for 80x86
ddk3790sp1.compiler       = $(ddk3790sp1.dir.bin)/x86/cl.exe
wdk.compiler              = $(ddk3790sp1.compiler)


ddk3790sp1.assembler      = $(ddk3790sp1.dir.bin)/x86/ml.exe
wdk.assembler             = $(ddk3790sp1.assembler)


ddk3790sp1.linker         = $(ddk3790sp1.dir.bin)/x86/link.exe
wdk.linker                = $(ddk3790sp1.linker)


ddk3790sp1.libtool        = $(ddk3790sp1.dir.bin)/x86/lib.exe
wdk.libtool               = $(ddk3790sp1.libtool)



ifeq      '$(target.kernel)'  'nt5.0'
  ddk3790sp1.dir.kernel = w2k
else ifeq '$(target.kernel)'  'nt5.1'
  ddk3790sp1.dir.kernel = wxp
else ifeq '$(target.kernel)'  'nt5.2'
  ddk3790sp1.dir.kernel = wnet
else 
  $(error target.kernel=$(target.kernel) is unsupported)
endif


wdk.dir.include.win32     = $(ddk3790sp1.dir.include)/$(ddk3790sp1.dir.kernel)
wdk.dir.include.crt       = $(ddk3790sp1.dir.include)/crt
wdk.dir.include.ddk       = $(ddk3790sp1.dir.include)/ddk/$(ddk3790sp1.dir.kernel)
wdk.dir.include.wdm       = $(ddk3790sp1.dir.include)/ddk/wdm/$(ddk3790sp1.dir.kernel)
wdk.dir.lib.wdm           = $(ddk3790sp1.dir.lib)/$(ddk3790sp1.dir.kernel)/i386


wdk.dir.include.1         = $(wdk.dir.include.win32)
wdk.dir.include.2         = $(wdk.dir.include.crt)
wdk.dir.include.3         = $(wdk.dir.include.ddk)
wdk.dir.include.4         = $(wdk.dir.include.wdm)
wdk.dir.lib.1             = $(wdk.dir.lib.wdm)



wdk.df                    = $(make.dir.bin)/x86/df.exe


target.arch = x86
wdk.version = 3790
toolset.host.arch = x86
toolset.family = ddk3790sp1
toolset.supported.kernels = nt5.0 nt5.1 nt5.2
toolset.not_supported.signing = 1
toolset.not_supported.RTC_and_GS = 1
toolset.not_supported.thiscall = 1


ifndef wdk.toolset.aux
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


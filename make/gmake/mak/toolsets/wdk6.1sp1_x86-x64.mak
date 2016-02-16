ifndef WDK61SP1_FOLDER
  $(error WDK61SP1_FOLDER is not defined)
endif


wdk61sp1.dir          = $(subst \,/,$(WDK61SP1_FOLDER))


wdk61sp1.dir.bin      = $(wdk61sp1.dir)/bin
wdk61sp1.dir.include  = $(wdk61sp1.dir)/inc
wdk61sp1.dir.lib      = $(wdk61sp1.dir)/lib
wdk61sp1.dir.sign     = $(wdk61sp1.dir.bin)/SelfSign


# Microsoft (R) C/C++ Optimizing Compiler Version 14.00.50727.278 for x64
wdk61sp1.compiler     = $(wdk61sp1.dir.bin)/x86/amd64/cl.exe
wdk.compiler          = $(wdk61sp1.compiler)


wdk61sp1.assembler    = $(wdk61sp1.dir.bin)/x86/amd64/ml64.exe
wdk.assembler         = $(wdk61sp1.assembler)


wdk61sp1.linker       = $(wdk61sp1.dir.bin)/x86/amd64/link.exe
wdk.linker            = $(wdk61sp1.linker)


wdk61sp1.libtool      = $(wdk61sp1.dir.bin)/x86/amd64/link.exe
wdk.libtool           = $(wdk61sp1.libtool)


wdk61sp1.signtool     = $(wdk61sp1.dir.sign)/signtool.exe
wdk.signtool          = $(wdk61sp1.signtool)


wdk61sp1.makecert     = $(wdk61sp1.dir.sign)/makecert.exe
wdk.makecert          = $(wdk61sp1.makecert)


wdk61sp1.certmgr      = $(wdk61sp1.dir.sign)/certmgr.exe
wdk.certmgr           = $(wdk61sp1.certmgr)


wdk61sp1.pvk2pfx      = $(wdk61sp1.dir.sign)/pvk2pfx.exe
wdk.pvk2pfx           = $(wdk61sp1.pvk2pfx)



#'$(target.kernel)' 'nt5.1' is unsupported for x64
ifeq        '$(target.kernel)'  'nt5.2'
  wdk61sp1.dir.lib.kernel = wnet
else ifeq   '$(target.kernel)'  'nt6.0'
  wdk61sp1.dir.lib.kernel = wlh
else 
  $(error target.kernel=$(target.kernel) is unsupported)
endif


wdk.dir.include.api     = $(wdk61sp1.dir.include)/api
wdk.dir.include.crt     = $(wdk61sp1.dir.include)/crt
wdk.dir.include.wdm     = $(wdk61sp1.dir.include)/ddk
wdk.dir.lib.wdm         = $(wdk61sp1.dir.lib)/$(wdk61sp1.dir.lib.kernel)/amd64
wdk.dir.include.kmdf    = $(wdk61sp1.dir.include)/wdf/kmdf/1.7
wdk.dir.lib.kmdf        = $(wdk61sp1.dir.lib)/wdf/kmdf/amd64/1.7

wdk.dir.include.1       = $(wdk.dir.include.api)
wdk.dir.include.2       = $(wdk.dir.include.crt)
wdk.dir.include.3       = $(wdk.dir.include.wdm)
ifdef module.kmdf
  wdk.dir.include.4     = $(wdk.dir.include.kmdf)
endif

wdk.dir.lib.1           = $(wdk.dir.lib.wdm)
ifdef module.kmdf
  wdk.dir.lib.2         = $(wdk.dir.lib.kmdf)
endif


wdk.df                = $(make.dir.bin)/x86/df.exe


target.arch = x64
wdk.version = 6001
toolset.host.arch = x86
toolset.family = wdk6.1sp1
toolset.supported.kernels = nt5.2 nt6.0
toolset.lib_link_shared = 1


ifndef wdk.toolset.aux
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


ifndef WDK710_FOLDER
  $(error WDK710_FOLDER is not defined)
endif


wdk710.dir            = $(subst \,/,$(WDK710_FOLDER))


wdk710.dir.bin        = $(wdk710.dir)/bin
wdk710.dir.include    = $(wdk710.dir)/inc
wdk710.dir.lib        = $(wdk710.dir)/lib


# Microsoft (R) C/C++ Optimizing Compiler Version 15.00.30729.207 for x64
wdk710.compiler       = $(wdk710.dir.bin)/x86/amd64/cl.exe
wdk.compiler          = $(wdk710.compiler)


wdk710.assembler      = $(wdk710.dir.bin)/x86/amd64/ml64.exe
wdk.assembler         = $(wdk710.assembler)


wdk710.linker         = $(wdk710.dir.bin)/x86/amd64/link.exe
wdk.linker            = $(wdk710.linker)


wdk710.libtool        = $(wdk710.dir.bin)/x86/amd64/link.exe
wdk.libtool           = $(wdk710.libtool)


wdk710.signtool       = $(wdk710.dir.bin)/x86/SignTool.exe
wdk.signtool          = $(wdk710.signtool)


wdk710.makecert       = $(wdk710.dir.bin)/x86/MakeCert.exe
wdk.makecert          = $(wdk710.makecert)


wdk710.certmgr        = $(wdk710.dir.bin)/x86/CertMgr.exe
wdk.certmgr           = $(wdk710.certmgr)


wdk710.pvk2pfx        = $(wdk710.dir.bin)/x86/pvk2pfx.exe
wdk.pvk2pfx           = $(wdk710.pvk2pfx)



#'$(target.kernel)' 'nt5.1' is unsupported for x64
ifeq      '$(target.kernel)'  'nt5.2'
  wdk710.dir.lib.kernel = wnet
else ifeq '$(target.kernel)'  'nt6.0'
  wdk710.dir.lib.kernel = wlh
else ifeq '$(target.kernel)'  'nt6.1'
  wdk710.dir.lib.kernel = win7
else 
  $(error target.kernel=$(target.kernel) is unsupported)
endif


wdk.dir.include.api       = $(wdk710.dir.include)/api
wdk.dir.include.crt       = $(wdk710.dir.include)/crt
wdk.dir.include.wdm       = $(wdk710.dir.include)/ddk
wdk.dir.lib.wdm           = $(wdk710.dir.lib)/$(wdk710.dir.lib.kernel)/amd64
wdk.dir.include.kmdf      = $(wdk710.dir.include)/wdf/kmdf/1.9
wdk.dir.lib.kmdf          = $(wdk710.dir.lib)/wdf/kmdf/amd64/1.9

wdk.dir.include.1         = $(wdk.dir.include.api)
wdk.dir.include.2         = $(wdk.dir.include.crt)
wdk.dir.include.3         = $(wdk.dir.include.wdm)
ifdef module.kmdf
  wdk.dir.include.4       = $(wdk.dir.include.kmdf)
endif

wdk.dir.lib.1             = $(wdk.dir.lib.wdm)
ifdef module.kmdf
  wdk.dir.lib.2           = $(wdk.dir.lib.kmdf)
endif


wdk.df                = $(make.dir.bin)/x86/df.exe


target.arch = x64
wdk.version = 7600
toolset.host.arch = x86
toolset.family = wdk7.1.0
toolset.supported.kernels = nt5.2 nt6.0 nt6.1
toolset.lib_link_shared = 1


ifndef wdk.toolset.aux
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


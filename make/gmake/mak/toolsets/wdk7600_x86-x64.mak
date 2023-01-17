# Windows 7 WDK 7.1.0.7600

ifndef WDK7600_FOLDER
  $(error define WDK7600_FOLDER)
endif


wdk7600.dir          = $(subst \,/,$(WDK7600_FOLDER))


wdk7600.dir.bin      = $(wdk7600.dir)/bin
wdk7600.dir.include  = $(wdk7600.dir)/inc
wdk7600.dir.lib      = $(wdk7600.dir)/lib


# Microsoft (R) C/C++ Optimizing Compiler Version 15.00.30729.207 for x64
wdk7600.compiler     = $(wdk7600.dir.bin)/x86/amd64/cl.exe
wdk.compiler         = $(wdk7600.compiler)

# Microsoft (R) Macro Assembler (x64) Version 9.00.30729.207
wdk7600.assembler    = $(wdk7600.dir.bin)/x86/amd64/ml64.exe
wdk.assembler        = $(wdk7600.assembler)

# Microsoft (R) Incremental Linker Version 9.00.30729.207
wdk7600.linker       = $(wdk7600.dir.bin)/x86/amd64/link.exe
wdk.linker           = $(wdk7600.linker)

# Microsoft (R) Incremental Linker Version 9.00.30729.207
wdk7600.libmgr       = $(wdk7600.dir.bin)/x86/amd64/link.exe
wdk.libmgr           = $(wdk7600.libmgr)

wdk7600.signtool     = $(wdk7600.dir.bin)/x86/SignTool.exe
wdk.signtool         = $(wdk7600.signtool)

wdk7600.makecert     = $(wdk7600.dir.bin)/x86/MakeCert.exe
wdk.makecert         = $(wdk7600.makecert)

wdk7600.certmgr      = $(wdk7600.dir.bin)/x86/CertMgr.exe
wdk.certmgr          = $(wdk7600.certmgr)

wdk7600.pvk2pfx      = $(wdk7600.dir.bin)/x86/pvk2pfx.exe
wdk.pvk2pfx          = $(wdk7600.pvk2pfx)


wdk7600.dir.lib.nt5.2  = wnet
wdk7600.dir.lib.nt6.0  = wlh
wdk7600.dir.lib.nt6.1  = win7


wdk.dir.include.api    = $(wdk7600.dir.include)/api
wdk.dir.include.crt    = $(wdk7600.dir.include)/crt
wdk.dir.include.wdm    = $(wdk7600.dir.include)/ddk
wdk.dir.lib.wdm        = $(wdk7600.dir.lib)/$(wdk7600.dir.lib.$(target.kernel))/amd64
wdk.dir.include.kmdf   = $(wdk7600.dir.include)/wdf/kmdf/1.9
wdk.dir.lib.kmdf       = $(wdk7600.dir.lib)/wdf/kmdf/amd64/1.9

sdk.dir.include.1      = $(wdk.dir.include.api)
sdk.dir.include.2      = $(wdk.dir.include.crt)
sdk.dir.include.3      = $(wdk.dir.include.wdm)
sdk.dir.include.4      = $(wdk.dir.include.kmdf)

sdk.dir.lib.1          = $(wdk.dir.lib.wdm)
sdk.dir.lib.2          = $(wdk.dir.lib.kmdf)


wdk.df = $(make.dir.bin)/x86/df.exe


wdk.version = 7600
target.arch = x64
toolset.arch = x86
toolset.family = wdk7600
toolset.supported.kernels = nt5.2 nt6.0 nt6.1
toolset.lib_link_shared = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


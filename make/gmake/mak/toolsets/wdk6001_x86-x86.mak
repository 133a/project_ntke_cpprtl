# Windows Vista SP1 WDK 6.1.6001

ifndef WDK6001_FOLDER
  $(error define WDK6001_FOLDER)
endif


wdk6001.dir          = $(subst \,/,$(WDK6001_FOLDER))


wdk6001.dir.bin      = $(wdk6001.dir)/bin
wdk6001.dir.include  = $(wdk6001.dir)/inc
wdk6001.dir.lib      = $(wdk6001.dir)/lib
wdk6001.dir.sign     = $(wdk6001.dir.bin)/SelfSign


# Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 14.00.50727.278 for 80x86
wdk6001.compiler     = $(wdk6001.dir.bin)/x86/x86/cl.exe
wdk.compiler         = $(wdk6001.compiler)

# Microsoft (R) Macro Assembler Version 8.00.50727.278
wdk6001.assembler    = $(wdk6001.dir.bin)/x86/ml.exe
wdk.assembler        = $(wdk6001.assembler)

# Microsoft (R) Incremental Linker Version 8.00.50727.278
wdk6001.linker       = $(wdk6001.dir.bin)/x86/x86/link.exe
wdk.linker           = $(wdk6001.linker)

# Microsoft (R) Incremental Linker Version 8.00.50727.278
wdk6001.libmgr       = $(wdk6001.dir.bin)/x86/x86/link.exe
wdk.libmgr           = $(wdk6001.libmgr)

wdk6001.signtool     = $(wdk6001.dir.sign)/signtool.exe
wdk.signtool         = $(wdk6001.signtool)

wdk6001.makecert     = $(wdk6001.dir.sign)/makecert.exe
wdk.makecert         = $(wdk6001.makecert)

wdk6001.certmgr      = $(wdk6001.dir.sign)/certmgr.exe
wdk.certmgr          = $(wdk6001.certmgr)

wdk6001.pvk2pfx      = $(wdk6001.dir.sign)/pvk2pfx.exe
wdk.pvk2pfx          = $(wdk6001.pvk2pfx)


wdk6001.dir.lib.nt5.0  = w2k
wdk6001.dir.lib.nt5.1  = wxp
wdk6001.dir.lib.nt5.2  = wnet
wdk6001.dir.lib.nt6.0  = wlh


wdk.dir.include.api    = $(wdk6001.dir.include)/api
wdk.dir.include.crt    = $(wdk6001.dir.include)/crt
wdk.dir.include.wdm    = $(wdk6001.dir.include)/ddk
wdk.dir.lib.wdm        = $(wdk6001.dir.lib)/$(wdk6001.dir.lib.$(target.kernel))/i386
wdk.dir.include.kmdf   = $(wdk6001.dir.include)/wdf/kmdf/1.7
wdk.dir.lib.kmdf       = $(wdk6001.dir.lib)/wdf/kmdf/i386/1.7

sdk.dir.include.1      = $(wdk.dir.include.api)
sdk.dir.include.2      = $(wdk.dir.include.crt)
sdk.dir.include.3      = $(wdk.dir.include.wdm)
sdk.dir.include.4      = $(wdk.dir.include.kmdf)

sdk.dir.lib.1          = $(wdk.dir.lib.wdm)
sdk.dir.lib.2          = $(wdk.dir.lib.kmdf)


wdk.df = $(make.dir.bin)/x86/df.exe


wdk.version = 6001
target.arch = x86
toolset.arch = x86
toolset.family = wdk6001
toolset.supported.kernels = nt5.0 nt5.1 nt5.2 nt6.0
toolset.lib_link_shared = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif



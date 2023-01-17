
msvc.dir.bin      = $(msvc.dir)/bin/Hostx64/x64
msvc.dir.include  = $(msvc.dir)/include

wdk.compiler      = $(msvc.dir.bin)/cl.exe
wdk.assembler     = $(msvc.dir.bin)/ml64.exe
wdk.linker        = $(msvc.dir.bin)/link.exe
wdk.libmgr        = $(msvc.dir.bin)/lib.exe


winsdk.dir.bin    = $(winsdk.dir)/bin/$(winsdk.version)/x64

wdk.signtool      = $(winsdk.dir.bin)/signtool.exe
wdk.makecert      = $(winsdk.dir.bin)/makecert.exe
wdk.certmgr       = $(winsdk.dir.bin)/certmgr.exe
wdk.pvk2pfx       = $(winsdk.dir.bin)/pvk2pfx.exe


wdk.dir.lib.nt10.0      = $(winsdk.version)


wdk.dir.include.km      = $(winsdk.dir)/Include/$(winsdk.version)/km
wdk.dir.include.crt     = $(winsdk.dir)/Include/$(winsdk.version)/km/crt
wdk.dir.include.ucrt    = $(winsdk.dir)/Include/$(winsdk.version)/ucrt
wdk.dir.include.shared  = $(winsdk.dir)/Include/$(winsdk.version)/shared
wdk.dir.include.um      = $(winsdk.dir)/Include/$(winsdk.version)/um
wdk.dir.lib             = $(winsdk.dir)/Lib/$(wdk.dir.lib.$(target.kernel))/km/x64


sdk.dir.include.1       = $(msvc.dir.include)
sdk.dir.include.2       = $(wdk.dir.include.km)
sdk.dir.include.3       = $(wdk.dir.include.ucrt)
sdk.dir.include.4       = $(wdk.dir.include.crt)
sdk.dir.include.5       = $(wdk.dir.include.shared)
sdk.dir.lib.1           = $(wdk.dir.lib)


wdk.df = $(make.dir.bin)/x64/df.exe


target.arch = x64
toolset.arch = x64
toolset.family = ewdk$(ewdk.version)-km
toolset.supported.kernels = nt10.0
toolset.supported.thread_safe_static = 1
toolset.supported.force_aslr = 1
toolset.supported.FH4 = 1
toolset.supported.FH4_default = 1
toolset.definitions += NT_INLINE_GET_CURRENT_IRQL


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


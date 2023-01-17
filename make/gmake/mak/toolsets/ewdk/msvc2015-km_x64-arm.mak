
msvc.dir.bin      = $(msvc.dir)/bin/amd64_arm
msvc.dir.include  = $(msvc.dir)/include

wdk.compiler      = $(msvc.dir.bin)/cl.exe
wdk.armasm        = $(msvc.dir.bin)/armasm.exe
wdk.linker        = $(msvc.dir.bin)/link.exe
wdk.libmgr        = $(msvc.dir.bin)/lib.exe


winsdk.dir.bin    = $(winsdk.dir)/bin/x64

wdk.signtool      = $(winsdk.dir.bin)/signtool.exe
wdk.makecert      = $(winsdk.dir.bin)/makecert.exe
wdk.certmgr       = $(winsdk.dir.bin)/certmgr.exe
wdk.pvk2pfx       = $(winsdk.dir.bin)/pvk2pfx.exe


wdk.dir.lib.nt6.2       = win8
wdk.dir.lib.nt6.3       = winv6.3
wdk.dir.lib.nt10.0      = $(winsdk.version)


wdk.dir.include.km      = $(winsdk.dir)/Include/$(winsdk.version)/km
wdk.dir.include.crt     = $(winsdk.dir)/Include/$(winsdk.version)/km/crt
wdk.dir.include.ucrt    = $(winsdk.dir)/Include/$(winsdk.version)/ucrt
wdk.dir.include.shared  = $(winsdk.dir)/Include/$(winsdk.version)/shared
wdk.dir.include.um      = $(winsdk.dir)/Include/$(winsdk.version)/um
wdk.dir.lib             = $(winsdk.dir)/Lib/$(wdk.dir.lib.$(target.kernel))/km/arm


sdk.dir.include.1       = $(msvc.dir.include)
sdk.dir.include.2       = $(wdk.dir.include.km)
sdk.dir.include.3       = $(wdk.dir.include.ucrt)
sdk.dir.include.4       = $(wdk.dir.include.crt)
sdk.dir.include.5       = $(wdk.dir.include.shared)
sdk.dir.lib.1           = $(wdk.dir.lib)


wdk.df = $(make.dir.bin)/x64/df.exe


target.arch = arm
toolset.arch = x64
toolset.family = ewdk$(ewdk.version)-km
toolset.supported.kernels = nt6.2 nt6.3 nt10.0
toolset.supported.thread_safe_static = 1
toolset.definitions = _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


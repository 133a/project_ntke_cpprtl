
msvc.dir.bin      = $(msvc.dir)/bin/Hostx86/x64
msvc.dir.include  = $(msvc.dir)/include

wdk.compiler      = $(msvc.dir.bin)/cl.exe
wdk.assembler     = $(msvc.dir.bin)/ml64.exe
wdk.linker        = $(msvc.dir.bin)/link.exe
wdk.libmgr        = $(msvc.dir.bin)/lib.exe


winsdk.dir.bin    = $(winsdk.dir)/bin/$(winsdk.version)/x86

wdk.signtool      = $(winsdk.dir.bin)/signtool.exe
wdk.makecert      = $(winsdk.dir.bin)/makecert.exe
wdk.certmgr       = $(winsdk.dir.bin)/certmgr.exe
wdk.pvk2pfx       = $(winsdk.dir.bin)/pvk2pfx.exe


#'$(target.kernel)' < 'nt6.1' is unsupported
ifeq      '$(target.kernel)'  'nt6.1'
  wdk.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk.dir.lib.kernel = win8
else ifeq '$(target.kernel)'  'nt6.3'
  wdk.dir.lib.kernel = winv6.3
else ifeq '$(target.kernel)'  'nt10.0'
  wdk.dir.lib.kernel = $(winsdk.version)
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk.dir.include.km      = $(winsdk.dir)/Include/$(winsdk.version)/km
wdk.dir.include.crt     = $(winsdk.dir)/Include/$(winsdk.version)/km/crt
wdk.dir.include.ucrt    = $(winsdk.dir)/Include/$(winsdk.version)/ucrt
wdk.dir.include.shared  = $(winsdk.dir)/Include/$(winsdk.version)/shared
wdk.dir.include.um      = $(winsdk.dir)/Include/$(winsdk.version)/um
wdk.dir.lib             = $(winsdk.dir)/Lib/$(wdk.dir.lib.kernel)/km/x64


sdk.dir.include.1       = $(msvc.dir.include)
sdk.dir.include.2       = $(wdk.dir.include.km)
sdk.dir.include.3       = $(wdk.dir.include.ucrt)
sdk.dir.include.4       = $(wdk.dir.include.crt)
sdk.dir.include.5       = $(wdk.dir.include.shared)
sdk.dir.lib.1           = $(wdk.dir.lib)


wdk.df                  = $(make.dir.bin)/x86/df.exe


target.arch = x64
toolset.arch = x86
toolset.family = ewdk$(ewdk.version)-km
toolset.supported.kernels = nt6.1 nt6.2 nt6.3 nt10.0
toolset.supported.thread_safe_static = 1
toolset.supported.force_aslr = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


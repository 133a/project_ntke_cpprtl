ifndef EWDK16299_FOLDER
  $(error define EWDK16299_FOLDER)
endif


ewdk16299.dir              = $(subst \,/,$(EWDK16299_FOLDER))

msvc2017.dir               = $(ewdk16299.dir)/Program Files/Microsoft Visual Studio/2017/BuildTools/VC/Tools/MSVC/14.11.25503
msvc2017.dir.bin           = $(msvc2017.dir)/bin/Hostx86/x86
msvc2017.dir.include       = $(msvc2017.dir)/include
msvc.dir.include           = $(msvc2017.dir.include)


# Microsoft (R) C/C++ Optimizing Compiler Version 19.11.25547 for x86
msvc2017.compiler          = $(msvc2017.dir.bin)/cl.exe
wdk.compiler               = $(msvc2017.compiler)

msvc2017.assembler         = $(msvc2017.dir.bin)/ml.exe
wdk.assembler              = $(msvc2017.assembler)

msvc2017.linker            = $(msvc2017.dir.bin)/link.exe
wdk.linker                 = $(msvc2017.linker)

msvc2017.libtool           = $(msvc2017.dir.bin)/lib.exe
wdk.libtool                = $(msvc2017.libtool)


winsdk10.dir               = $(ewdk16299.dir)/Program Files/Windows Kits/10
winsdk10.build             = 10.0.16299.0
winsdk10.dir.bin           = $(winsdk10.dir)/bin/$(winsdk10.build)/x86


winsdk10.signtool          = $(winsdk10.dir.bin)/signtool.exe
wdk.signtool               = $(winsdk10.signtool)

winsdk10.makecert          = $(winsdk10.dir.bin)/makecert.exe
wdk.makecert               = $(winsdk10.makecert)

winsdk10.certmgr           = $(winsdk10.dir.bin)/certmgr.exe
wdk.certmgr                = $(winsdk10.certmgr)

winsdk10.pvk2pfx           = $(winsdk10.dir.bin)/pvk2pfx.exe
wdk.pvk2pfx                = $(winsdk10.pvk2pfx)


#'$(target.kernel)' < 'nt6.1' is unsupported
ifeq      '$(target.kernel)'  'nt6.1'
  wdk10.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk10.dir.lib.kernel = win8
else ifeq '$(target.kernel)'  'nt6.3'
  wdk10.dir.lib.kernel = winv6.3
else ifeq '$(target.kernel)'  'nt10.0'
  wdk10.dir.lib.kernel = $(winsdk10.build)
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk10.dir.include.km       = $(winsdk10.dir)/Include/$(winsdk10.build)/km
wdk10.dir.include.crt      = $(winsdk10.dir)/Include/$(winsdk10.build)/km/crt
wdk10.dir.include.ucrt     = $(winsdk10.dir)/Include/$(winsdk10.build)/ucrt
wdk10.dir.include.shared   = $(winsdk10.dir)/Include/$(winsdk10.build)/shared
wdk10.dir.include.um       = $(winsdk10.dir)/Include/$(winsdk10.build)/um
wdk10.dir.lib              = $(winsdk10.dir)/Lib/$(wdk10.dir.lib.kernel)/km/x86


sdk.dir.include.1          = $(msvc.dir.include)
sdk.dir.include.2          = $(wdk10.dir.include.km)
sdk.dir.include.3          = $(wdk10.dir.include.ucrt)
sdk.dir.include.4          = $(wdk10.dir.include.crt)
sdk.dir.include.5          = $(wdk10.dir.include.shared)
sdk.dir.lib.1              = $(wdk10.dir.lib)


wdk.df                     = $(make.dir.bin)/x86/df.exe


wdk.version = 16299
target.arch = x86
toolset.arch = x86
toolset.family = ewdk16299-km
toolset.supported.kernels = nt6.1 nt6.2 nt6.3 nt10.0
toolset.supported.thread_safe_static = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


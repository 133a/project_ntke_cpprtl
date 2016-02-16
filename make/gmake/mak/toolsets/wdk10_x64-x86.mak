ifndef MSVS2015_FOLDER
  $(error MSVS2015_FOLDER is not defined)
endif


msvs2015.dir               = $(subst \,/,$(MSVS2015_FOLDER))

msvc2015.dir               = $(msvs2015.dir)/VC
msvc2015.dir.bin           = $(msvc2015.dir)/bin/amd64_x86
msvc2015.dir.include       = $(msvc2015.dir)/include
msvc.dir.include           = $(msvc2015.dir.include)


# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.23026 for x86
msvc2015.compiler          = $(msvc2015.dir.bin)/cl.exe
wdk.compiler               = $(msvc2015.compiler)


msvc2015.assembler         = $(msvc2015.dir.bin)/ml.exe
wdk.assembler              = $(msvc2015.assembler)


msvc2015.linker            = $(msvc2015.dir.bin)/link.exe
wdk.linker                 = $(msvc2015.linker)

                           
msvc2015.libtool           = $(msvc2015.dir.bin)/lib.exe
wdk.libtool                = $(msvc2015.libtool)



ifndef WINSDK_10_FOLDER
  $(error WINSDK_10_FOLDER is not defined)
endif
winsdk_10.dir              = $(subst \,/,$(WINSDK_10_FOLDER))
winsdk_10.build            = 10.0.10586.0
wdk10.build                = 10.0.10240.0

winsdk_10.dir.bin          = $(winsdk_10.dir)/bin

winsdk_10.signtool         = $(winsdk_10.dir.bin)/x64/signtool.exe
wdk.signtool               = $(winsdk_10.signtool)

winsdk_10.makecert         = $(winsdk_10.dir.bin)/x64/makecert.exe
wdk.makecert               = $(winsdk_10.makecert)

winsdk_10.certmgr          = $(winsdk_10.dir.bin)/x64/certmgr.exe
wdk.certmgr                = $(winsdk_10.certmgr)

winsdk_10.pvk2pfx          = $(winsdk_10.dir.bin)/x64/pvk2pfx.exe
wdk.pvk2pfx                = $(winsdk_10.pvk2pfx)


#'$(target.kernel)' < 'nt6.1' is unsupported
ifeq      '$(target.kernel)'  'nt6.1'
  wdk10.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk10.dir.lib.kernel = win8
else ifeq '$(target.kernel)'  'nt6.3'
  wdk10.dir.lib.kernel = winv6.3
else ifeq '$(target.kernel)'  'nt10.0'
  wdk10.dir.lib.kernel = $(wdk10.build)
else 
  $(error target.kernel=$(target.kernel) is unsupported)
endif


wdk10.dir.include.km       = $(winsdk_10.dir)/Include/$(wdk10.build)/km
wdk10.dir.include.crt      = $(winsdk_10.dir)/Include/$(wdk10.build)/km/crt
wdk10.dir.include.ucrt     = $(winsdk_10.dir)/Include/$(winsdk_10.build)/ucrt
wdk10.dir.include.shared   = $(winsdk_10.dir)/Include/$(winsdk_10.build)/shared
wdk10.dir.include.um       = $(winsdk_10.dir)/Include/$(winsdk_10.build)/um
wdk10.dir.lib              = $(winsdk_10.dir)/Lib/$(wdk10.dir.lib.kernel)/km/x86


wdk.dir.include.1          = $(msvc.dir.include)
wdk.dir.include.2          = $(wdk10.dir.include.km)
wdk.dir.include.3          = $(wdk10.dir.include.ucrt)
wdk.dir.include.4          = $(wdk10.dir.include.crt)
wdk.dir.include.5          = $(wdk10.dir.include.shared)
wdk.dir.lib.1              = $(wdk10.dir.lib)


wdk.df                     = $(make.dir.bin)/x64/df.exe


target.arch = x86
wdk.version = 10240
toolset.host.arch = x64
toolset.family = wdk10
toolset.supported.kernels = nt6.1 nt6.2 nt6.3 nt10.0
toolset.supported.thread_safe_static = 1


ifndef wdk.toolset.aux
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


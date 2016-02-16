ifndef MSVS2013_FOLDER
  $(error MSVS2013_FOLDER is not defined)
endif


msvs2013.dir               = $(subst \,/,$(MSVS2013_FOLDER))

msvc2013.dir               = $(msvs2013.dir)/VC
msvc2013.dir.bin           = $(msvc2013.dir)/bin/amd64
msvc2013.dir.include       = $(msvc2013.dir)/include
msvc.dir.include           = $(msvc2013.dir.include)
                           

# Microsoft (R) C/C++ Optimizing Compiler Version 18.00.21005.1 for x64
msvc2013.compiler          = $(msvc2013.dir.bin)/cl.exe
wdk.compiler               = $(msvc2013.compiler)


msvc2013.assembler         = $(msvc2013.dir.bin)/ml64.exe
wdk.assembler              = $(msvc2013.assembler)


msvc2013.linker            = $(msvc2013.dir.bin)/link.exe
wdk.linker                 = $(msvc2013.linker)

                           
msvc2013.libtool           = $(msvc2013.dir.bin)/lib.exe
wdk.libtool                = $(msvc2013.libtool)



ifndef WINSDK_8.1_FOLDER
  $(error WINSDK_8.1_FOLDER is not defined)
endif

winsdk_8.1.dir             = $(subst \,/,$(WINSDK_8.1_FOLDER))
winsdk_8.1.dir.bin         = $(winsdk_8.1.dir)/bin

winsdk_8.1.signtool        = $(winsdk_8.1.dir.bin)/x64/signtool.exe
wdk.signtool               = $(winsdk_8.1.signtool)

winsdk_8.1.makecert        = $(winsdk_8.1.dir.bin)/x64/makecert.exe
wdk.makecert               = $(winsdk_8.1.makecert)

winsdk_8.1.certmgr         = $(winsdk_8.1.dir.bin)/x64/certmgr.exe
wdk.certmgr                = $(winsdk_8.1.certmgr)

winsdk_8.1.pvk2pfx         = $(winsdk_8.1.dir.bin)/x64/pvk2pfx.exe
wdk.pvk2pfx                = $(winsdk_8.1.pvk2pfx)



#'$(target.kernel)' < 'nt6.1' is unsupported
ifeq      '$(target.kernel)'  'nt6.1'
  wdk81.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk81.dir.lib.kernel = win8
else ifeq '$(target.kernel)'  'nt6.3'
  wdk81.dir.lib.kernel = winv6.3
else 
  $(error target.kernel=$(target.kernel) is unsupported)
endif


wdk81.dir.include.km       = $(winsdk_8.1.dir)/Include/km
wdk81.dir.include.crt      = $(winsdk_8.1.dir)/Include/km/crt
wdk81.dir.include.shared   = $(winsdk_8.1.dir)/Include/shared
wdk81.dir.include.um       = $(winsdk_8.1.dir)/Include/um
wdk81.dir.lib              = $(winsdk_8.1.dir)/Lib/$(wdk81.dir.lib.kernel)/km/x64


wdk.dir.include.1          = $(msvc.dir.include)
wdk.dir.include.2          = $(wdk81.dir.include.km)
wdk.dir.include.3          = $(wdk81.dir.include.crt)
wdk.dir.include.4          = $(wdk81.dir.include.shared)
# for macamd64.inc
wdk.dir.include.5          = $(wdk81.dir.include.um)
wdk.dir.lib.1              = $(wdk81.dir.lib)


wdk.df                     = $(make.dir.bin)/x64/df.exe


target.arch = x64
wdk.version = 9600
toolset.host.arch = x64
toolset.family = wdk8.1
toolset.supported.kernels = nt6.1 nt6.2 nt6.3


ifndef wdk.toolset.aux
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


ifndef MSVS2013_FOLDER
  $(error define MSVS2013_FOLDER)
endif


msvs2013.dir               = $(subst \,/,$(MSVS2013_FOLDER))

msvc2013.dir               = $(msvs2013.dir)/VC
msvc2013.dir.bin           = $(msvc2013.dir)/bin/x86_arm
msvc2013.dir.include       = $(msvc2013.dir)/include
msvc.dir.include           = $(msvc2013.dir.include)
                           

# Microsoft (R) C/C++ Optimizing Compiler Version 18.00.21005.1 for ARM
# Microsoft (R) C/C++ Optimizing Compiler Version 18.00.40629 for ARM (upd5)
msvc2013.compiler          = $(msvc2013.dir.bin)/cl.exe
wdk.compiler               = $(msvc2013.compiler)

msvc2013.armasm            = $(msvc2013.dir.bin)/armasm.exe
wdk.armasm                 = $(msvc2013.armasm)

msvc2013.linker            = $(msvc2013.dir.bin)/link.exe
wdk.linker                 = $(msvc2013.linker)

msvc2013.libtool           = $(msvc2013.dir.bin)/lib.exe
wdk.libtool                = $(msvc2013.libtool)


ifndef WINSDK8.1_FOLDER
  $(error define WINSDK8.1_FOLDER)
endif

winsdk8.1.dir              = $(subst \,/,$(WINSDK8.1_FOLDER))
winsdk8.1.dir.bin          = $(winsdk8.1.dir)/bin

winsdk8.1.signtool         = $(winsdk8.1.dir.bin)/x86/signtool.exe
wdk.signtool               = $(winsdk8.1.signtool)

winsdk8.1.makecert         = $(winsdk8.1.dir.bin)/x86/makecert.exe
wdk.makecert               = $(winsdk8.1.makecert)

winsdk8.1.certmgr          = $(winsdk8.1.dir.bin)/x86/certmgr.exe
wdk.certmgr                = $(winsdk8.1.certmgr)

winsdk8.1.pvk2pfx          = $(winsdk8.1.dir.bin)/x86/pvk2pfx.exe
wdk.pvk2pfx                = $(winsdk8.1.pvk2pfx)


# '$(target.kernel)' < 'nt6.2' is unsupported
ifeq       '$(target.kernel)'  'nt6.2'
  wdk81.dir.lib.kernel = win8
else ifeq  '$(target.kernel)'  'nt6.3'
  wdk81.dir.lib.kernel = winv6.3
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk81.dir.include.km       = $(winsdk8.1.dir)/Include/km
wdk81.dir.include.crt      = $(winsdk8.1.dir)/Include/km/crt
wdk81.dir.include.shared   = $(winsdk8.1.dir)/Include/shared
wdk81.dir.lib              = $(winsdk8.1.dir)/Lib/$(wdk81.dir.lib.kernel)/km/arm


sdk.dir.include.1          = $(msvc.dir.include)
sdk.dir.include.2          = $(wdk81.dir.include.km)
sdk.dir.include.3          = $(wdk81.dir.include.crt)
sdk.dir.include.4          = $(wdk81.dir.include.shared)
sdk.dir.lib.1              = $(wdk81.dir.lib)


wdk.df                     = $(make.dir.bin)/x86/df.exe


wdk.version = 9600
target.arch = arm
toolset.arch = x86
toolset.family = wdk8.1
toolset.supported.kernels = nt6.2 nt6.3
toolset.definitions = _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


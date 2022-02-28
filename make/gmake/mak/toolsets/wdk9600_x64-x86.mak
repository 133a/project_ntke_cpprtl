# Windows 8.1 WDK 8.1.9600

ifndef MSVS2013_FOLDER
  $(error define MSVS2013_FOLDER)
endif


msvs2013.dir          = $(subst \,/,$(MSVS2013_FOLDER))

msvc2013.dir          = $(msvs2013.dir)/VC
msvc2013.dir.bin      = $(msvc2013.dir)/bin/amd64_x86
msvc2013.dir.include  = $(msvc2013.dir)/include
msvc.dir.include      = $(msvc2013.dir.include)


# Microsoft (R) C/C++ Optimizing Compiler Version 18.00.21005.1 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 18.00.40629 for x86 (msvs2013u5)
msvc2013.compiler     = $(msvc2013.dir.bin)/cl.exe
wdk.compiler          = $(msvc2013.compiler)

# Microsoft (R) Macro Assembler Version 12.00.40629.0 (msvs2013u5)
msvc2013.assembler    = $(msvc2013.dir.bin)/ml.exe
wdk.assembler         = $(msvc2013.assembler)

# Microsoft (R) Incremental Linker Version 12.00.40629.0 (msvs2013u5)
msvc2013.linker       = $(msvc2013.dir.bin)/link.exe
wdk.linker            = $(msvc2013.linker)

# Microsoft (R) Library Manager Version 12.00.40629.0 (msvs2013u5)
msvc2013.libmgr       = $(msvc2013.dir.bin)/lib.exe
wdk.libmgr            = $(msvc2013.libmgr)


ifndef WINSDK8.1_FOLDER
  $(error define WINSDK8.1_FOLDER)
endif

winsdk8.1.dir       = $(subst \,/,$(WINSDK8.1_FOLDER))
winsdk8.1.dir.bin   = $(winsdk8.1.dir)/bin

winsdk8.1.signtool  = $(winsdk8.1.dir.bin)/x64/signtool.exe
wdk.signtool        = $(winsdk8.1.signtool)

winsdk8.1.makecert  = $(winsdk8.1.dir.bin)/x64/makecert.exe
wdk.makecert        = $(winsdk8.1.makecert)

winsdk8.1.certmgr   = $(winsdk8.1.dir.bin)/x64/certmgr.exe
wdk.certmgr         = $(winsdk8.1.certmgr)

winsdk8.1.pvk2pfx   = $(winsdk8.1.dir.bin)/x64/pvk2pfx.exe
wdk.pvk2pfx         = $(winsdk8.1.pvk2pfx)


#'$(target.kernel)' < 'nt6.1' is unsupported
ifeq      '$(target.kernel)'  'nt6.1'
  wdk9600.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk9600.dir.lib.kernel = win8
else ifeq '$(target.kernel)'  'nt6.3'
  wdk9600.dir.lib.kernel = winv6.3
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk9600.dir.include.km      = $(winsdk8.1.dir)/Include/km
wdk9600.dir.include.crt     = $(winsdk8.1.dir)/Include/km/crt
wdk9600.dir.include.shared  = $(winsdk8.1.dir)/Include/shared
wdk9600.dir.lib             = $(winsdk8.1.dir)/Lib/$(wdk9600.dir.lib.kernel)/km/x86


sdk.dir.include.1           = $(msvc.dir.include)
sdk.dir.include.2           = $(wdk9600.dir.include.km)
sdk.dir.include.3           = $(wdk9600.dir.include.crt)
sdk.dir.include.4           = $(wdk9600.dir.include.shared)
sdk.dir.lib.1               = $(wdk9600.dir.lib)


wdk.df = $(make.dir.bin)/x64/df.exe


wdk.version = 9600
target.arch = x86
toolset.arch = x64
toolset.family = wdk9600
toolset.supported.kernels = nt6.1 nt6.2 nt6.3


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


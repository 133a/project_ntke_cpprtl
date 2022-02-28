# Windows 8 WDK 8.0.9200

ifndef MSVS2012_FOLDER
  $(error define MSVS2012_FOLDER)
endif


msvs2012.dir          = $(subst \,/,$(MSVS2012_FOLDER))

msvc2012.dir          = $(msvs2012.dir)/VC
msvc2012.dir.bin      = $(msvc2012.dir)/bin/x86_amd64
msvc2012.dir.include  = $(msvc2012.dir)/include
msvc.dir.include      = $(msvc2012.dir.include)
                           

# Microsoft (R) C/C++ Optimizing Compiler Version 17.00.50727.1 for x64                          
# Microsoft (R) C/C++ Optimizing Compiler Version 17.00.60610.1 for x64 (msvs2012u3)
# Microsoft (R) C/C++ Optimizing Compiler Version 17.00.61030 for x64 (msvs2012u5)
msvc2012.compiler     = $(msvc2012.dir.bin)/cl.exe
wdk.compiler          = $(msvc2012.compiler)

# Microsoft (R) Macro Assembler (x64) Version 11.00.61030.0 (msvs2012u5)
msvc2012.assembler    = $(msvc2012.dir.bin)/ml64.exe
wdk.assembler         = $(msvc2012.assembler)

# Microsoft (R) Incremental Linker Version 11.00.61030.0 (msvs2012u5)
msvc2012.linker       = $(msvc2012.dir.bin)/link.exe
wdk.linker            = $(msvc2012.linker)

# Microsoft (R) Library Manager Version 11.00.61030.0 (msvs2012u5)
msvc2012.libmgr       = $(msvc2012.dir.bin)/lib.exe
wdk.libmgr            = $(msvc2012.libmgr)


ifndef WINSDK8.0_FOLDER
  $(error define WINSDK8.0_FOLDER)
endif

winsdk8.0.dir       = $(subst \,/,$(WINSDK8.0_FOLDER))
winsdk8.0.dir.bin   = $(winsdk8.0.dir)/bin

winsdk8.0.signtool  = $(winsdk8.0.dir.bin)/x86/signtool.exe
wdk.signtool        = $(winsdk8.0.signtool)

winsdk8.0.makecert  = $(winsdk8.0.dir.bin)/x86/makecert.exe
wdk.makecert        = $(winsdk8.0.makecert)

winsdk8.0.certmgr   = $(winsdk8.0.dir.bin)/x86/certmgr.exe
wdk.certmgr         = $(winsdk8.0.certmgr)

winsdk8.0.pvk2pfx   = $(winsdk8.0.dir.bin)/x86/pvk2pfx.exe
wdk.pvk2pfx         = $(winsdk8.0.pvk2pfx)


#'$(target.kernel)' < 'nt6.0' is unsupported
ifeq      '$(target.kernel)'  'nt6.0'
  wdk9200.dir.lib.kernel = wlh
else ifeq '$(target.kernel)'  'nt6.1'
  wdk9200.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk9200.dir.lib.kernel = win8
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk9200.dir.include.km      = $(winsdk8.0.dir)/Include/km
wdk9200.dir.include.crt     = $(winsdk8.0.dir)/Include/km/crt
wdk9200.dir.include.shared  = $(winsdk8.0.dir)/Include/shared
wdk9200.dir.include.um      = $(winsdk8.0.dir)/Include/um
wdk9200.dir.lib             = $(winsdk8.0.dir)/Lib/$(wdk9200.dir.lib.kernel)/km/x64


sdk.dir.include.1           = $(msvc.dir.include)
sdk.dir.include.2           = $(wdk9200.dir.include.km)
sdk.dir.include.3           = $(wdk9200.dir.include.crt)
sdk.dir.include.4           = $(wdk9200.dir.include.shared)
# for macamd64.inc
sdk.dir.include.5           = $(wdk9200.dir.include.um)
sdk.dir.lib.1               = $(wdk9200.dir.lib)


wdk.df = $(make.dir.bin)/x86/df.exe


wdk.version = 9200
target.arch = x64
toolset.arch = x86
toolset.family = wdk9200
toolset.supported.kernels = nt6.0 nt6.1 nt6.2


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


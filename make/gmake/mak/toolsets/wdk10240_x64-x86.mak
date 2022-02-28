# Windows 10 WDK 10.0.10240

ifndef MSVS2015_FOLDER
  $(error define MSVS2015_FOLDER)
endif


msvs2015.dir          = $(subst \,/,$(MSVS2015_FOLDER))

msvc2015.dir          = $(msvs2015.dir)/VC
msvc2015.dir.bin      = $(msvc2015.dir)/bin/amd64_x86
msvc2015.dir.include  = $(msvc2015.dir)/include
msvc.dir.include      = $(msvc2015.dir.include)


# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.23026 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.23918 for x86 (msvs2015u2)
# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.24210 for x86 (msvs2015u3)
msvc2015.compiler     = $(msvc2015.dir.bin)/cl.exe
wdk.compiler          = $(msvc2015.compiler)

# Microsoft (R) Macro Assembler Version 14.00.24210.0 (msvs2015u3)
msvc2015.assembler    = $(msvc2015.dir.bin)/ml.exe
wdk.assembler         = $(msvc2015.assembler)

# Microsoft (R) Incremental Linker Version 14.00.24210.0 (msvs2015u3)
msvc2015.linker       = $(msvc2015.dir.bin)/link.exe
wdk.linker            = $(msvc2015.linker)

# Microsoft (R) Library Manager Version 14.00.24210.0 (msvs2015u3)
msvc2015.libmgr       = $(msvc2015.dir.bin)/lib.exe
wdk.libmgr            = $(msvc2015.libmgr)


ifndef WINSDK10_FOLDER
  $(error define WINSDK10_FOLDER)
endif
winsdk10.dir       = $(subst \,/,$(WINSDK10_FOLDER))
winsdk10.build     = 10.0.10586.0
wdk10240.build     = 10.0.10240.0

winsdk10.dir.bin   = $(winsdk10.dir)/bin

winsdk10.signtool  = $(winsdk10.dir.bin)/x64/signtool.exe
wdk.signtool       = $(winsdk10.signtool)

winsdk10.makecert  = $(winsdk10.dir.bin)/x64/makecert.exe
wdk.makecert       = $(winsdk10.makecert)

winsdk10.certmgr   = $(winsdk10.dir.bin)/x64/certmgr.exe
wdk.certmgr        = $(winsdk10.certmgr)

winsdk10.pvk2pfx   = $(winsdk10.dir.bin)/x64/pvk2pfx.exe
wdk.pvk2pfx        = $(winsdk10.pvk2pfx)


#'$(target.kernel)' < 'nt6.1' is unsupported
ifeq      '$(target.kernel)'  'nt6.1'
  wdk10240.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk10240.dir.lib.kernel = win8
else ifeq '$(target.kernel)'  'nt6.3'
  wdk10240.dir.lib.kernel = winv6.3
else ifeq '$(target.kernel)'  'nt10.0'
  wdk10240.dir.lib.kernel = $(wdk10240.build)
else 
  $(error unknown target.kernel=$(target.kernel))
endif


wdk10240.dir.include.km      = $(winsdk10.dir)/Include/$(wdk10240.build)/km
wdk10240.dir.include.crt     = $(winsdk10.dir)/Include/$(wdk10240.build)/km/crt
wdk10240.dir.include.ucrt    = $(winsdk10.dir)/Include/$(winsdk10.build)/ucrt
wdk10240.dir.include.shared  = $(winsdk10.dir)/Include/$(winsdk10.build)/shared
wdk10240.dir.include.um      = $(winsdk10.dir)/Include/$(winsdk10.build)/um
wdk10240.dir.lib             = $(winsdk10.dir)/Lib/$(wdk10240.dir.lib.kernel)/km/x86


sdk.dir.include.1            = $(msvc.dir.include)
sdk.dir.include.2            = $(wdk10240.dir.include.km)
sdk.dir.include.3            = $(wdk10240.dir.include.ucrt)
sdk.dir.include.4            = $(wdk10240.dir.include.crt)
sdk.dir.include.5            = $(wdk10240.dir.include.shared)
sdk.dir.lib.1                = $(wdk10240.dir.lib)


wdk.df = $(make.dir.bin)/x64/df.exe


wdk.version = 10240
target.arch = x86
toolset.arch = x64
toolset.family = wdk10240
toolset.supported.kernels = nt6.1 nt6.2 nt6.3 nt10.0
toolset.supported.thread_safe_static = 1


ifndef toolset.aux.type
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


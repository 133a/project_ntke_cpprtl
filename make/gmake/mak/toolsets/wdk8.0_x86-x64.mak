ifndef MSVS2012_FOLDER
  $(error MSVS2012_FOLDER is not defined)
endif


msvs2012.dir               = $(subst \,/,$(MSVS2012_FOLDER))

msvc2012.dir               = $(msvs2012.dir)/VC
msvc2012.dir.bin           = $(msvc2012.dir)/bin/x86_amd64
msvc2012.dir.include       = $(msvc2012.dir)/include
msvc.dir.include           = $(msvc2012.dir.include)
                           

# Microsoft (R) C/C++ Optimizing Compiler Version 17.00.50727.1 for x64                          
# Microsoft (R) C/C++ Optimizing Compiler Version 17.00.60610.1 for x64 (upd3)
# Microsoft (R) C/C++ Optimizing Compiler Version 17.00.61030 for x64 (upd4)
msvc2012.compiler          = $(msvc2012.dir.bin)/cl.exe
wdk.compiler               = $(msvc2012.compiler)


msvc2012.assembler         = $(msvc2012.dir.bin)/ml64.exe
wdk.assembler              = $(msvc2012.assembler)


msvc2012.linker            = $(msvc2012.dir.bin)/link.exe
wdk.linker                 = $(msvc2012.linker)

                           
msvc2012.libtool           = $(msvc2012.dir.bin)/lib.exe
wdk.libtool                = $(msvc2012.libtool)



ifndef WINSDK_8.0_FOLDER
  $(error WINSDK_8.0_FOLDER is not defined)
endif

winsdk_8.0.dir             = $(subst \,/,$(WINSDK_8.0_FOLDER))
winsdk_8.0.dir.bin         = $(winsdk_8.0.dir)/bin

winsdk_8.0.signtool        = $(winsdk_8.0.dir.bin)/x86/signtool.exe
wdk.signtool               = $(winsdk_8.0.signtool)

winsdk_8.0.makecert        = $(winsdk_8.0.dir.bin)/x86/makecert.exe
wdk.makecert               = $(winsdk_8.0.makecert)

winsdk_8.0.certmgr         = $(winsdk_8.0.dir.bin)/x86/certmgr.exe
wdk.certmgr                = $(winsdk_8.0.certmgr)

winsdk_8.0.pvk2pfx         = $(winsdk_8.0.dir.bin)/x86/pvk2pfx.exe
wdk.pvk2pfx                = $(winsdk_8.0.pvk2pfx)



#'$(target.kernel)' < 'nt6.0' is unsupported
ifeq      '$(target.kernel)'  'nt6.0'
  wdk80.dir.lib.kernel = wlh
else ifeq '$(target.kernel)'  'nt6.1'
  wdk80.dir.lib.kernel = win7
else ifeq '$(target.kernel)'  'nt6.2'
  wdk80.dir.lib.kernel = win8
else 
  $(error target.kernel=$(target.kernel) is unsupported)
endif


wdk80.dir.include.km       = $(winsdk_8.0.dir)/Include/km
wdk80.dir.include.crt      = $(winsdk_8.0.dir)/Include/km/crt
wdk80.dir.include.shared   = $(winsdk_8.0.dir)/Include/shared
wdk80.dir.include.um       = $(winsdk_8.0.dir)/Include/um
wdk80.dir.lib              = $(winsdk_8.0.dir)/Lib/$(wdk80.dir.lib.kernel)/km/x64


wdk.dir.include.1          = $(msvc.dir.include)
wdk.dir.include.2          = $(wdk80.dir.include.km)
wdk.dir.include.3          = $(wdk80.dir.include.crt)
wdk.dir.include.4          = $(wdk80.dir.include.shared)
# for macamd64.inc
wdk.dir.include.5          = $(wdk80.dir.include.um)
wdk.dir.lib.1              = $(wdk80.dir.lib)


wdk.df                     = $(make.dir.bin)/x86/df.exe


target.arch = x64
wdk.version = 9200
toolset.host.arch = x86
toolset.family = wdk8.0
toolset.supported.kernels = nt6.0 nt6.1 nt6.2


ifndef wdk.toolset.aux
  include $(make.dir.toolsets)/wdk/wdk.mak
endif


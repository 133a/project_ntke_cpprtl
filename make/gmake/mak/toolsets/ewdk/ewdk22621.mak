# EWDK_ni_release_svc_prod1_22621_220804-1759
#
# Microsoft (R) C/C++ Optimizing Compiler Version 19.31.31107 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 19.31.31107 for x64
# Microsoft (R) C/C++ Optimizing Compiler Version 19.31.31107 for ARM
# Microsoft (R) C/C++ Optimizing Compiler Version 19.31.31107 for ARM64
#
# Microsoft (R) Macro Assembler Version 14.31.31107.0
# Microsoft (R) Macro Assembler (x64) Version 14.31.31107.0
# Microsoft (R) ARM Macro Assembler Version 14.31.31107.0
# Microsoft (R) ARM Macro Assembler Version 14.31.31107.0 for 64 bits
#
# Microsoft (R) Incremental Linker Version 14.31.31107.0
# 
# Microsoft (R) Library Manager Version 14.31.31107.0


ewdk.version    = 22621
wdk.version     = $(ewdk.version)
msvc.version    = 14.31.31103
winsdk.version  = 10.0.$(ewdk.version).0


ifndef EWDK$(ewdk.version)_FOLDER
  $(error define EWDK$(ewdk.version)_FOLDER)
endif

ewdk.dir        = $(subst \,/,$(EWDK$(ewdk.version)_FOLDER))
msvc.dir        = $(ewdk.dir)/Program Files/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/$(msvc.version)
winsdk.dir      = $(ewdk.dir)/Program Files/Windows Kits/10

toolset.signtool_requires_digest_alg = 1

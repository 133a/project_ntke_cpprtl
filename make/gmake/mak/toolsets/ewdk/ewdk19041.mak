# EWDK_vb_release_svc_prod1_19041_201201-2105
#
# Microsoft (R) C/C++ Optimizing Compiler Version 19.27.29110 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 19.27.29110 for x64
# Microsoft (R) C/C++ Optimizing Compiler Version 19.27.29110 for ARM
# Microsoft (R) C/C++ Optimizing Compiler Version 19.27.29110 for ARM64
#
# Microsoft (R) Macro Assembler Version 14.27.29110.0
# Microsoft (R) Macro Assembler (x64) Version 14.27.29110.0
# Microsoft (R) ARM Macro Assembler Version 14.27.29110.0
# Microsoft (R) ARM Macro Assembler Version 14.27.29110.0 for 64 bits
#
# Microsoft (R) Incremental Linker Version 14.27.29110.0
# 
# Microsoft (R) Library Manager Version 14.27.29110.0


ewdk.version    = 19041
wdk.version     = $(ewdk.version)
msvc.version    = 14.27.29110
winsdk.version  = 10.0.$(ewdk.version).0


ifndef EWDK$(ewdk.version)_FOLDER
  $(error define EWDK$(ewdk.version)_FOLDER)
endif

ewdk.dir        = $(subst \,/,$(EWDK$(ewdk.version)_FOLDER))
msvc.dir        = $(ewdk.dir)/Program Files/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC/$(msvc.version)
winsdk.dir      = $(ewdk.dir)/Program Files/Windows Kits/10

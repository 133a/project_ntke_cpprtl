# EWDK_rs5_release_svc_prod2_17763_190129-1747
#
# Microsoft (R) C/C++ Optimizing Compiler Version 19.15.26732.1 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 19.15.26732.1 for x64
# Microsoft (R) C/C++ Optimizing Compiler Version 19.15.26732.1 for ARM
# Microsoft (R) C/C++ Optimizing Compiler Version 19.15.26732.1 for ARM64
#
# Microsoft (R) Macro Assembler Version 14.15.26732.1
# Microsoft (R) Macro Assembler (x64) Version 14.15.26732.1
# Microsoft (R) ARM Macro Assembler Version 14.15.26732.1
# Microsoft (R) ARM Macro Assembler Version 14.15.26732.1 for 64 bits
#
# Microsoft (R) Incremental Linker Version 14.15.26732.1
#
# Microsoft (R) Library Manager Version 14.15.26732.1


ewdk.version    = 17763
wdk.version     = $(ewdk.version)
msvc.version    = 14.15.26726
winsdk.version  = 10.0.$(ewdk.version).0


ifndef EWDK$(ewdk.version)_FOLDER
  $(error define EWDK$(ewdk.version)_FOLDER)
endif

ewdk.dir        = $(subst \,/,$(EWDK$(ewdk.version)_FOLDER))
msvc.dir        = $(ewdk.dir)/Program Files/Microsoft Visual Studio/2017/BuildTools/VC/Tools/MSVC/$(msvc.version)
winsdk.dir      = $(ewdk.dir)/Program Files/Windows Kits/10

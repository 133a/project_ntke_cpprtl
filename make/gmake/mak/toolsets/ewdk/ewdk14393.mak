# EWDK_rs1_release_14393_20160715-1616
#
# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.24210 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.24210 for x64
# Microsoft (R) C/C++ Optimizing Compiler Version 19.00.24210 for ARM
#
# Microsoft (R) Macro Assembler Version 14.00.24210.0
# Microsoft (R) Macro Assembler (x64) Version 14.00.24210.0
# Microsoft (R) ARM Macro Assembler Version 14.00.24210.0
# 
# Microsoft (R) Incremental Linker Version 14.00.24210.0
#
# Microsoft (R) Library Manager Version 14.00.24210.0


ewdk.version    = 14393
wdk.version     = $(ewdk.version)
winsdk.version  = 10.0.$(ewdk.version).0


ifndef EWDK$(ewdk.version)_FOLDER
  $(error define EWDK$(ewdk.version)_FOLDER)
endif

ewdk.dir        = $(subst \,/,$(EWDK$(ewdk.version)_FOLDER))
msvc.dir        = $(ewdk.dir)/Program Files/Microsoft Visual Studio 14.0/VC
winsdk.dir      = $(ewdk.dir)/Program Files/Windows Kits/10

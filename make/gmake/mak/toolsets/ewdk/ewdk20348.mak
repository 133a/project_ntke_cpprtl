# EWDK_fe_release_20348_210507-1500
#
# Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29913 for x86
# Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29913 for x64
# Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29913 for ARM
# Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29913 for ARM64
#
# Microsoft (R) Macro Assembler Version 14.28.29913.0
# Microsoft (R) Macro Assembler (x64) Version 14.28.29913.0
# Microsoft (R) ARM Macro Assembler Version 14.28.29913.0
# Microsoft (R) ARM Macro Assembler Version 14.28.29913.0 for 64 bits
#
# Microsoft (R) Incremental Linker Version 14.28.29913.0
# 
# Microsoft (R) Library Manager Version 14.28.29913.0


ewdk.version    = 20348
wdk.version     = $(ewdk.version)
msvc.version    = 14.28.29910
winsdk.version  = 10.0.$(ewdk.version).0


ifndef EWDK$(ewdk.version)_FOLDER
  $(error define EWDK$(ewdk.version)_FOLDER)
endif

ewdk.dir        = $(subst \,/,$(EWDK$(ewdk.version)_FOLDER))
msvc.dir        = $(ewdk.dir)/Program Files/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC/$(msvc.version)
winsdk.dir      = $(ewdk.dir)/Program Files/Windows Kits/10

# ewdk20348 recommends only to explicitely set the digest algo, but we force the reqirement
toolset.signtool_requires_digest_alg = 1


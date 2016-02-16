icl.wdk = wdk7.1.0_x64-x64
icl.aux = icl_wdk

###  wdk7.1.0_x64-x64 is not a true x64 toolset - the compiler, linker and lib are cross-x86-x64
###  icl13.0_x64-x64 needs x64 mspdb*.dll (that is absent in the fake x64 wdk toolset) if '-Zi' option is used, so let's just override this option
icl.compiler.options.cpp += -Z7

include $(make.dir.toolsets)/icl_win/icl13.0_x64-x64.mak

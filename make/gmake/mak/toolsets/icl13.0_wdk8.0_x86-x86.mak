icl.wdk = wdk8.0_x86-x86
icl.aux = icl_wdk

wdk.aux.compiler.definitions += NO_KERNEL_LIST_ENTRY_CHECKS

include $(make.dir.toolsets)/icl_win/icl13.0_x86-x86.mak

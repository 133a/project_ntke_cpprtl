###############################################
#      armasm definitions
###############################################

  wdk.armasm.definitions += NT_KERNEL_MODE
  wdk.armasm.definitions += DDK_VER=$(wdk.version)



###############################################
#      target.profile armasm options
###############################################

  wdk.armasm.options.debug            =
  wdk.armasm.options.release          =
  wdk.armasm.options.cpprtl_debug     =
  wdk.armasm.options.cpprtl_release   =



###############################################
#      include the armasm engine
###############################################

include $(msvc.dir.toolset)/impl/armasm.mak


###############################################
#      masm definitions
###############################################

  wdk.assembler.options += -DNT_KERNEL_MODE
  wdk.assembler.options += -DDDK_VER=$(wdk.version)



###############################################
#      target.profile options
###############################################

  wdk.assembler.options.debug            =
  wdk.assembler.options.release          =
  wdk.assembler.options.cpprtl_debug     =
  wdk.assembler.options.cpprtl_release   =



###############################################
#      include the masm engine
###############################################

include $(msvc.dir.toolset)/impl/masm.mak


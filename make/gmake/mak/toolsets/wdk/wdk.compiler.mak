###############################################
#      wdk.compiler.definitions
###############################################

  wdk.compiler.definitions += NT_KERNEL_MODE
  wdk.compiler.definitions += DDK_VER=$(wdk.version)



###############################################
#      include target.profile variables
###############################################

include $(wdk.dir.toolset)/target.profile/$(target.profile).cl.mak



###############################################
#      include sdk variables
###############################################

include $(make.dir.toolsets)/_winnt/sdk.defs.winnt.mak



###############################################
#      include the compiler engine
###############################################

include $(msvc.dir.toolset)/impl/cl.mak


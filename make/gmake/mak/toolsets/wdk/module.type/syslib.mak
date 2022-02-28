include $(make.dir.toolsets)/_win32/target.mak


###############################################
#      the target file
###############################################

  target.name = $(module.name)
  target.ext  = $(toolset.ext.lib)


  target.file           = $(target.dir.bin)/$(target.name)$(target.ext)
  target.main.depends  += target.dirs
  target.main.depends  += $(target.file)



###############################################
#      module default values
###############################################

# src search directories
  module.dir.sources ?= .



###############################################
#      composing build engines
###############################################

include $(make.dir.toolsets)/_win32/obj.mak
include $(wdk.dir.toolset)/wdk.assembler.mak
include $(wdk.dir.toolset)/wdk.compiler.mak
include $(wdk.dir.toolset)/wdk.libmgr.mak
include $(make.dir.util)/mkdir.win32.mak


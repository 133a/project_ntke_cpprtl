  target.name = $(module.name)
  target.ext  = .lib


include $(wdk.dir.toolset)/obj.mak
include $(wdk.dir.toolset)/wdk.assembler.mak
include $(wdk.dir.toolset)/wdk.compiler.mak
include $(wdk.dir.toolset)/wdk.libtool.mak
include $(make.dir.util)/mkdir.win32.mak


  target.name = $(module.name)
  target.ext  = .sys


include $(wdk.dir.toolset)/obj.mak
include $(wdk.dir.toolset)/wdk.assembler.mak
include $(wdk.dir.toolset)/wdk.compiler.mak
include $(wdk.dir.toolset)/wdk.linker.mak

ifndef toolset.not_supported.signing
  ifndef module.no_signing
    include $(wdk.dir.toolset)/wdk.signtool.mak
  endif
endif

include $(make.dir.util)/mkdir.win32.mak

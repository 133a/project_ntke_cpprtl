  toolset.supported.FH4 = 1
  toolset.supported.FH4_default = 1
  toolset.definitions += NT_INLINE_GET_CURRENT_IRQL

include $(make.dir.toolsets)/ewdk/ewdk20348.mak
include $(make.dir.toolsets)/ewdk/msvc2017,2019-km_x86-x64.mak


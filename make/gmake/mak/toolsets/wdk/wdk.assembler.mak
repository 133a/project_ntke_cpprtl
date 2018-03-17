###############################################
###    choosing the arch-specific assembler
###############################################

ifeq      '$(target.arch)' 'x86'
  include $(make.dir.toolsets)/wdk/wdk.masm.mak
else ifeq '$(target.arch)' 'x64'
  include $(make.dir.toolsets)/wdk/wdk.masm.mak
else ifeq '$(target.arch)' 'arm'
  include $(make.dir.toolsets)/wdk/wdk.armasm.mak
else ifeq '$(target.arch)' 'arm64'
  include $(make.dir.toolsets)/wdk/wdk.armasm64.mak
else ifeq '$(target.arch)' 'ia64'
  include $(make.dir.toolsets)/wdk/wdk.ias.mak
else
  $(error unknown target.arch=$(target.arch))
endif


###############################################
#    toolset definitions
###############################################

  toolset.type      = wdk
  target.platform   = win32
  wdk.dir.toolset   = $(make.dir.toolsets)/wdk
  msvc.dir.toolset  = $(make.dir.toolsets)/msvc

  toolset.no_default_kernel = 1

  toolset.ext.pdb   = .pdb
  toolset.ext.obj   = .obj
  toolset.ext.lib   = .lib
  toolset.ext.sys   = .sys

  toolset.enum      = $(make.enum)

# turn off the dependency filter
# toolset.no_df = 1


###############################################
#   module definitions
###############################################

include ./$(target.makefile)


# checking the necessary module incoming vars
ifndef module.name
  $(error define module.name))
endif
ifndef module.type
  $(error define module.type))
endif



###############################################
#    target definitions
###############################################

include $(wdk.dir.toolset)/module.type/$(module.type).mak


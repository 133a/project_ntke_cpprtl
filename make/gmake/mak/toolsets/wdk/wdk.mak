  wdk.dir.toolset = $(make.dir.toolsets)/wdk


#checking the necessary toolset & target incoming vars
ifndef target.arch
  $(error define target.arch))
endif
ifndef target.kernel
  $(error define target.kernel))
endif
ifndef target.profile
  $(error define target.profile))
endif
ifndef toolset.family
  $(error define toolset.family))
endif
ifndef toolset.supported.kernels
  $(error define toolset.supported.kernels))
endif


#checking the target.kernel
$(if $(filter $(target.kernel), $(toolset.supported.kernels)) \
,                                                             \
, $(error target.kernel=$(target.kernel) is unsupported)      \
)


####### include the module definitions #######
include $(target.makefile)
#######


#checking the necessary module incoming vars
ifndef module.name
  $(error define module.name))
endif
ifndef module.type
  $(error define module.type))
endif


  wdk.module.enum = $(target.arch) \
                    $(target.kernel) \
                    $(target.arch).$(target.kernel) \
                    $(toolset.family) \
                    $(toolset.family).$(target.arch) \
                    $(toolset.family).$(target.kernel) \
                    $(toolset.family).$(target.arch).$(target.kernel)


# c/c++ include directories
  project.dir.include.1 = $(project.dir.root)
  project.dir.include.2 = $(project.dir.root)/include


# forwarding parameters for recursive make invokes
  target.make.forward += $(call var_forward,target.toolset)
  target.make.forward += $(call var_forward,target.profile)
  target.make.forward += $(call var_forward,target.kernel)
  target.make.forward += $(call var_forward,module.EHcpp)
  target.make.forward += $(call var_forward,module.RTTI)
  target.make.forward += $(call var_forward,module.no_signing)


include $(wdk.dir.toolset)/module.type/$(module.type).mak


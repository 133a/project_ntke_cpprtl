### checking the toolset necessary incoming vars
ifndef toolset.type
  $(error define toolset.type))
endif
ifndef toolset.family
  $(error define toolset.family))
endif
ifndef toolset.supported.kernels
  $(error define toolset.supported.kernels))
endif


### checking the target necessary incoming vars
ifndef target.profile
  $(error define target.profile))
endif
ifndef target.arch
  $(error define target.arch))
endif


### define the default target.kernel
ifndef toolset.no_default_kernel
  target.kernel ?= $(firstword $(toolset.supported.kernels))
endif


### checking the target.kernel
$(if $(filter $(target.kernel),$(toolset.supported.kernels)),,$(error unknown target.kernel=$(target.kernel)))


### define the default target.charset
ifdef toolset.supported.charsets
  target.charset ?= $(firstword $(toolset.supported.charsets))
  $(if $(filter $(target.charset),$(toolset.supported.charsets)),,$(error unknown target.charset=$(target.charset)))
endif


### collect the target.enum
  target.enum  = $(target.kernel) \
                 $(target.arch) \
                 $(target.kernel)-$(target.arch) \
                 $(target.platform) \
                 $(target.platform)-$(target.arch) \
                 $(toolset.type) \
                 $(toolset.type).$(target.kernel) \
                 $(toolset.type).$(target.arch) \
                 $(toolset.type).$(target.kernel)-$(target.arch) \
                 $(toolset.family) \
                 $(toolset.family).$(target.kernel) \
                 $(toolset.family).$(target.arch) \
                 $(toolset.family).$(target.kernel)-$(target.arch)
ifdef toolset.aux.type
  ifndef toolset.aux.family
    $(error define toolset.aux.family))
  endif
  target.enum += $(toolset.aux.type) \
                 $(toolset.aux.type).$(target.kernel) \
                 $(toolset.aux.type).$(target.arch) \
                 $(toolset.aux.type).$(target.kernel)-$(target.arch) \
                 $(toolset.aux.family) \
                 $(toolset.aux.family).$(target.kernel) \
                 $(toolset.aux.family).$(target.arch) \
                 $(toolset.aux.family).$(target.kernel)-$(target.arch)
endif


### target output folders
  target.dir.spec    = $(target.profile)_$(target.arch)_$(target.kernel)$(if $(target.charset),_$(target.charset),)
  target.dir.output  = $(project.dir.output)/$(target.name)$(target.ext)/$(target.toolset)/$(target.dir.spec)


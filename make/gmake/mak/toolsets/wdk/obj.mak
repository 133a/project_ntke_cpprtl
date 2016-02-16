#checking the necessary target vars
ifndef target.profile
  $(error define target.profile))
endif


#target params
  target.dir.build  = $(project.dir.output)/$(target.name)$(target.ext)/$(target.toolset)/$(target.profile)_$(target.arch)_$(target.kernel)
  target.dir.obj    = $(target.dir.build)/obj


  target.sources     = $(call enum2list_with_base,$(wdk.module.enum),module.sources,,)
  target.objects     = $(addprefix $(target.dir.obj)/, $(addsuffix .obj, $(basename $(notdir $(target.sources)))))


#collecting output directories
  target.main.depends           += target.current.dirs
  target.current.dirs.depends   += $(target.dir.build) $(target.dir.obj)


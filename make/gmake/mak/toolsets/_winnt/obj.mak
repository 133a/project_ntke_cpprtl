### project common include directories
  project.dir.include.1 = $(project.dir.root)
  project.dir.include.2 = $(project.dir.root)/include


### collecting target source & object files
  target.sources     = $(call enum2list_with_base,$(target.enum),module.sources,,)
  target.resources   = $(call enum2list_with_base,$(target.enum),module.resources,,)
  target.objects     = $(addprefix $(target.dir.obj)/,$(addsuffix $(toolset.ext.obj),$(basename $(notdir $(target.sources)))))
  target.objects    += $(addprefix $(target.dir.obj)/,$(addsuffix $(toolset.ext.res),$(basename $(notdir $(target.resources)))))


### add output folders to the dependencies list
  target.dir.bin         = $(target.dir.output)
  target.dir.obj         = $(target.dir.bin)/obj
  target.dirs.depends   += $(target.dir.bin)
  target.dirs.depends   += $(target.dir.obj)


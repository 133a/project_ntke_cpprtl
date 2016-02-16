#creating output directories
.PHONY : target.current.dirs
target.current.dirs : $(target.current.dirs.depends)


$(target.current.dirs.depends) :
	if not exist $(call winslash,$@) mkdir $(call winslash,$@)


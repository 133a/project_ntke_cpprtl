#creating output directories
.PHONY : target.dirs
target.dirs : $(target.dirs.depends)


$(target.dirs.depends) :
	cmd /c if not exist $(call winslash,$@) mkdir $(call winslash,$@)


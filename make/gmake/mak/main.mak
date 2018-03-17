  make.dir = $(project.dir.root)/make/gmake
  make.dir.bin = $(make.dir)/bin
  make.dir.bat = $(make.dir)/bat
  make.dir.mak = $(make.dir)/mak
  make.dir.toolsets = $(make.dir.mak)/toolsets
  make.dir.util     = $(make.dir.mak)/util
  make.makefile = GNUmakefile
  make.enum = 1 2 3 4 5 6 7 8 9
  make.make = $(MAKE)


include $(make.dir.mak)/func.mak


  project.dir.output = $(project.dir.root)/__output
  target.makefile = module.mak


#the default target
.PHONY : target.main
target.main :


include $(make.dir.toolsets)/$(target.toolset).mak


target.main : $(target.main.depends)  # collects a sequence of build steps


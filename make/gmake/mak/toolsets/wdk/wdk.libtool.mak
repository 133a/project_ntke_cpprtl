  target.filename = $(target.dir.build)/$(target.name)$(target.ext)
  target.main.depends += $(target.filename)


ifdef toolset.lib_link_shared
  wdk.libtool.options += -LIB
endif

  wdk.libtool.options += -NOLOGO -LIBPATH:"$(target.dir.obj)"



$(target.filename) : $(target.objects)
	"$(wdk.libtool)" $(wdk.libtool.options) -OUT:"$@" $(^F)


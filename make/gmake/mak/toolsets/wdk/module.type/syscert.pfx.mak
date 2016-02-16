  target.name = $(module.name)
  target.ext = .pfx


  target.dir.build  = .
  target.filename   = $(target.dir.build)/$(target.name)$(target.ext)


  target.main.depends += $(target.filename)


ifndef toolset.not_supported.signing
  include $(wdk.dir.toolset)/wdk.certtools.mak
endif

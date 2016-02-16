.PHONY : sign_file
  target.main.depends += sign_file


sign_file :
	"$(wdk.signtool)" sign -v -f $(module.cert.file.pfx) $(target.dir.build)/$(target.name)$(target.ext)

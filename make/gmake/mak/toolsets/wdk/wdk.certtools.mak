#add the current certificate into the store
.PHONY : add_cert
add_cert : $(target.filename)
	"$(wdk.certmgr)" -add -c $^ -s -r localmachine root
	"$(wdk.certmgr)" -add -c $^ -s -r localmachine trustedpublisher


#generate the certificate (default)
$(target.filename) :
	"$(wdk.makecert)" -r -pe -n "CN=$(module.name)" -sv $(@:$(target.ext)=.pvk) $(@:$(target.ext)=.cer)
	"$(wdk.pvk2pfx)" -pvk $(@:$(target.ext)=.pvk) -spc $(@:$(target.ext)=.cer) -pfx $@
	del $(@:$(target.ext)=.pvk)
	del $(@:$(target.ext)=.cer)


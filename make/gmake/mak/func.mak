
# $1 - enumeration list, $2 - var base name, $3 - prefix $4 - suffix
define enum2list_spaced
$(strip $(foreach idx,$1,$(if $($2.$(idx)),$3$($2.$(idx))$4,)))
endef



# $1 - enumeration list, $2 - var base name, $3 - prefix $4 - suffix
define enum2list_with_base_spaced
$(strip $(if $($2),$3$($2)$4,) $(call enum2list,$1,$2,$3,$4))
endef



# $1 - enumeration list, $2 - var base name, $3 - prefix $4 - suffix
define enum2list
$(strip $(foreach idx,$1,$(if $($2.$(idx)),$($2.$(idx):%=$3%$4),)))
endef



# $1 - enumeration list, $2 - var base name, $3 - prefix $4 - suffix
define enum2list_with_base
$(strip $(if $($2),$($2:%=$3%$4),) $(call enum2list,$1,$2,$3,$4))
endef



# $1 - variable to forward
define var_forward
$(if $($1),$1=$($1),)
endef



define winslash
$(subst /,\,$1)
endef


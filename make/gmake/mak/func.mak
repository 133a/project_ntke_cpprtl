# $1 - string
define winslash
$(subst /,\,$1)
endef


# $1 - enumeration list
# $2 - var base name
# $3 - prefix
# $4 - suffix
define enum2list
$(strip $(foreach idx,$1,$(if $($2.$(idx)),$($2.$(idx):%=$3%$4),)))
endef


# $1 - enumeration list
# $2 - var base name
# $3 - prefix
# $4 - suffix
define enum2list_with_base
$(strip $(if $($2),$($2:%=$3%$4),) $(call enum2list,$1,$2,$3,$4))
endef


# $1 - enumeration list
# $2 - var base name
# $3 - prefix
# $4 - suffix
define enum2list_spaced
$(strip $(foreach idx,$1,$(if $($2.$(idx)),$3$($2.$(idx))$4,)))
endef


# $1 - enumeration list
# $2 - var base name
# $3 - prefix
# $4 - suffix
define enum2list_with_base_spaced
$(strip $(if $($2),$3$($2)$4,) $(call enum2list_spaced,$1,$2,$3,$4))
endef


# $1 - enumeration list
# $2 - var base name
# $3 - filter
define enum2list_filtered
$(strip $(foreach idx,$1,$(if $($2.$(idx)),$(call $3,$2.$(idx)),)))
endef


# $1 - enumeration list
# $2 - var base name
# $3 - filter
define enum2list_with_base_filtered
$(strip $(if $($2),$(call $3,$($2)),) $(call enum2list_filtered,$1,$2,$3,$4))
endef


# $1 - enumeration list
# $2 - var base name
# $3 - filter
# $4 - separator
define enum2string_filtered
$(strip $(eval make.var.tmp:=)$(eval make.var.ret:=)                                                                                               \
  $(foreach idx,$1,                                                                                                                                \
    $(eval make.var.ret:=$(make.var.ret)$(if $($2.$(idx)),$(make.var.tmp)$(call $3,$($2.$(idx)))$(eval make.var.tmp:=$4),$(eval make.var.tmp:=)))  \
  )                                                                                                                                                \
  $(make.var.ret)                                                                                                                                  \
)
endef


# $1 - enumeration list
# $2 - var base name
# $3 - filter
# $4 - separator
define enum2string_with_base_filtered
$(strip $(if $($2),$(call $3,$($2))$4,)$(call enum2string_filtered,$1,$2,$3,$4))
endef


define make.filter.forward
$1
endef

define make.filter.quote
"$1"
endef

define make.filter.escaped_quote
\"$1\"
endef


make.var.LC = a b c d e f g h i j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z
make.var.UC = A B C D E F G H I J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z
make.var.N  = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26

# $1 - string to be lowered
define make.fun.lower_case
$(strip                                                                                                       \
  $(eval make.var.ret := $1)                                                                                  \
  $(foreach n,$(make.var.N),                                                                                  \
    $(eval make.var.ret := $(subst $(word $(n),$(make.var.UC)),$(word $(n),$(make.var.LC)),$(make.var.ret)))  \
  )                                                                                                           \
  $(make.var.ret)                                                                                             \
)
endef


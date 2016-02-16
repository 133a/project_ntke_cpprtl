module.name = ntke_eh
module.type = syslib


module.path.base = ..


ifndef module.EHcpp
module.EHcpp = a
endif


include ./module.src.mak


# ke-mode specific:
module.sources.arm += eh_restore_context.arm.asm

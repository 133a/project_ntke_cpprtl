module.name = ntke_eh
module.type = syslib


module.dir.base = ..


include ./module.src.mak


# ke-mode specific:
module.sources.arm += eh_restore_context.arm.asm

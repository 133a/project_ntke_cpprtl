ifndef INTELCL13.0_FOLDER
  $(error INTELCL13.0_FOLDER is not defined)
endif


icl13.0.dir            = $(subst \,/,$(INTELCL13.0_FOLDER))
                           
                           
icl13.0.dir.bin        = $(icl13.0.dir)/bin/intel64
icl13.0.dir.include    = $(icl13.0.dir)/compiler/include
icl13.0.dir.lib        = $(icl13.0.dir)/compiler/lib/intel64

icl13.0.compiler       = $(icl13.0.dir.bin)/icl.exe
icl.compiler           = $(icl13.0.compiler)

icl13.0.linker         = $(icl13.0.dir.bin)/xilink.exe
icl.linker             = $(icl13.0.linker)

icl13.0.libtool        = $(icl13.0.dir.bin)/xilib.exe
icl.libtool            = $(icl13.0.libtool)



icl.dir.include.1      = $(icl13.0.dir.include)
icl.dir.lib.1          = $(icl13.0.dir.lib)



icl.compiler.options.cpp  += -Qdiag-disable:1885
icl.compiler.options.c    += $(icl.compiler.options.cpp)



include $(make.dir.toolsets)/icl_win/$(icl.aux).mak


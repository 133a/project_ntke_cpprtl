ifndef icl.wdk
  $(error icl.wdk is not defined)
endif
  wdk.toolset.aux = 1
include $(make.dir.toolsets)/$(icl.wdk).mak



  wdk.compiler  =  $(icl.compiler)
  wdk.linker    =  $(icl.linker)
ifndef toolset.lib_link_shared       # xilib.exe is trying to execute 'lib.exe' so let's just workaround this misbehavior
  wdk.libtool   =  $(icl.libtool)
endif


  wdk.aux.compiler.definitions     +=

  wdk.aux.compiler.includes.prev   += $(call enum2list_spaced,$(make.enum),icl.dir.include,-I",")
  wdk.aux.compiler.includes.post   +=

  wdk.aux.linker.libpath.prev      += $(call enum2list_spaced,$(make.enum),icl.dir.lib,-LIBPATH:",")
  wdk.aux.linker.libpath.post      +=


# the '-Ox' or '-Ot' optimizations lead to the enormous stack wastage
  wdk.aux.compiler.optimize.release   = -Ox
# switch '-Qfreestanding' is to unmake the ICL of referencing the optimized functions from 'libirc*.lib'
  wdk.aux.compiler.options.cpp       += $(icl.compiler.options.cpp) -Qdiag-disable:161 -Qfreestanding
  wdk.aux.compiler.options.c         += $(icl.compiler.options.c) -Qfreestanding
  wdk.aux.linker.options             += $(icl.linker.options)
  wdk.aux.libtool.options            += $(icl.libtool.options)



include $(make.dir.toolsets)/wdk/wdk.mak


###############################################
#      cl definitions
###############################################

  wdk.compiler.definitions.cpprtl_release         +=  NDEBUG



###############################################
#      cl options
###############################################

#optimization
  wdk.compiler.options.cpprtl_release             +=  -Ox


#code generation                               
  wdk.compiler.options.cpprtl_release             +=  -GF

  wdk.compiler.options.cpprtl_release.x86         +=  -Gz

  wdk.compiler.options.cpp.cpprtl_release         +=  -GR
ifdef module.EHa
  wdk.compiler.options.cpp.cpprtl_release         +=  -EHa
else
  wdk.compiler.options.cpp.cpprtl_release         +=  -EHs
endif


#debugging
  wdk.compiler.options.cpprtl_release             +=  -Zi
ifndef toolset.not_supported.RTC_and_GS
  wdk.compiler.options.cpprtl_release             +=  -GS-
endif


#language
  wdk.compiler.options.cpprtl_release             +=  -Zc:forScope
  wdk.compiler.options.cpprtl_release             +=  -Zc:wchar_t
  wdk.compiler.options.cpprtl_release             +=  -Zl
  wdk.compiler.options.cpprtl_release             +=  -Zp8
ifdef toolset.supported.thread_safe_static
  wdk.compiler.options.cpprtl_release             +=  -Zc:threadSafeInit-
endif


#miscellaneous
  wdk.compiler.options.cpprtl_release             +=  -J
  wdk.compiler.options.cpprtl_release             +=  -W3
  wdk.compiler.options.cpprtl_release             +=  -nologo


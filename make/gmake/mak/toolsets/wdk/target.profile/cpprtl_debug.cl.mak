###############################################
#      cl definitions
###############################################

  wdk.compiler.definitions.cpprtl_debug            +=  _DEBUG DEBUG DBG



###############################################
#      cl options
###############################################

#optimization
  wdk.compiler.options.cpprtl_debug                +=  -Od


#code generation
  wdk.compiler.options.cpprtl_debug                +=  -GF

  wdk.compiler.options.cpprtl_debug.x86            +=  -Gz

  wdk.compiler.options.cpp.cpprtl_debug            +=  -GR
ifdef module.EHa
  wdk.compiler.options.cpp.cpprtl_debug            +=  -EHa
else
  wdk.compiler.options.cpp.cpprtl_debug            +=  -EHs
endif


#debugging
  wdk.compiler.options.cpprtl_debug                +=  -Zi
ifndef toolset.not_supported.RTC_and_GS
  wdk.compiler.options.cpprtl_debug                +=  -GS-
endif


#language
  wdk.compiler.options.cpprtl_debug                +=  -Zc:forScope
  wdk.compiler.options.cpprtl_debug                +=  -Zc:wchar_t
  wdk.compiler.options.cpprtl_debug                +=  -Zl
  wdk.compiler.options.cpprtl_debug                +=  -Zp8
ifdef toolset.supported.thread_safe_static
  wdk.compiler.options.cpprtl_debug                +=  -Zc:threadSafeInit-
endif


#miscellaneous
  wdk.compiler.options.cpprtl_debug                +=  -J
  wdk.compiler.options.cpprtl_debug                +=  -W3
  wdk.compiler.options.cpprtl_debug                +=  -nologo


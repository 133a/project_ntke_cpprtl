//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef NTKE_STLPORT_CFG_H_
#define NTKE_STLPORT_CFG_H_


#if defined (_STLP_PLATFORM)
#  undef _STLP_PLATFORM
#endif
#define _STLP_PLATFORM  "NTKE"


// stl config
#define _NOTHREADS 1
#define _STLP_NO_IOSTREAMS 1
#define _STLP_NO_CUSTOM_IO 1
#define _STLP_USE_NEWALLOC 1
#define _STLP_USE_PTR_SPECIALIZATIONS 1
#define _STLP_USE_NEW_C_HEADERS 1

#if !defined (_STLP_VENDOR_GLOBAL_CSTD)
#  define _STLP_VENDOR_GLOBAL_CSTD 1
#endif

#if defined (NTDDI_VERSION) && (NTDDI_VERSION < 0x06010000)
#  if defined (_STLP_USE_SAFE_STRING_FUNCTIONS)
#    undef _STLP_USE_SAFE_STRING_FUNCTIONS
#  endif
#  ifdef _MSC_VER
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#endif 

#ifdef _STLP_HAS_INCLUDE_NEXT
#  undef _STLP_HAS_INCLUDE_NEXT
#endif
#define _STLP_DONT_USE_INCLUDE_NEXT  //  for icl


// header files config
#if defined (DDK_VER)  // ddk/wdk toolset

#  if    (DDK_VER < 9000)    //  the DDK/WDK prior to the msvc-integrated versions
#    define  _STLP_NATIVE_C_INCLUDE_PATH          ../crt
#  elif  (DDK_VER < 10000)   //  wdk9200 wdk9600
#    define  _STLP_NATIVE_C_INCLUDE_PATH          ../include
#  elif  (DDK_VER >= 10240)  // wdk10240 ewdk*
#    define  _STLP_NATIVE_C_INCLUDE_PATH          ke_mode/winnt/ntke_cpprtl/std_incl/internal_/stlport_supp/cstd_headers_redirect
#  else
#    error unsupported DDK_VER
#  endif
#else                  // msvc toolset
#  if (_MSC_VER >= 1900)  // >=msvc2015
#    define  _STLP_NATIVE_C_INCLUDE_PATH          ke_mode/winnt/ntke_cpprtl/std_incl/internal_/stlport_supp/cstd_headers_redirect
#  else
#    define  _STLP_NATIVE_C_INCLUDE_PATH          ../include
#  endif

#endif  // DDK_VER


#define  _STLP_NATIVE_CPP_C_INCLUDE_PATH        ke_mode/winnt/ntke_cpprtl/std_incl/internal_/stlport_supp/cppstd_headers_redirect
#define  _STLP_NATIVE_CPP_RUNTIME_INCLUDE_PATH  ke_mode/winnt/ntke_cpprtl/std_incl


// ucrt compatibility
#if (DDK_VER > 10240)  // ewdk*
#  define __NOTHROW_T_DEFINED
#  define _INC_CRTDBG  // prevent WDK/<crtdbg.h> to be included to avoid CRT/<vcruntime_new.h> conflicting stuff
#endif


#endif  // include guard

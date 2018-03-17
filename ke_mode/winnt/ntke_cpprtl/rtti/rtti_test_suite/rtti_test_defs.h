/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_TEST_DEFS_H_
#define RTTI_TEST_DEFS_H_


#ifdef _MSC_FULL_VER

#  if ( _MSC_FULL_VER < 140050727 )
#    define MSVC_OLD_STYLE_RTTI  // msvc cl.exe < v14.00.50727 (msvc2005) lacks BC_WITH_HIERARCHY
#  endif

#  if ( _MSC_FULL_VER >= 190024210 )
#    define MSVC_2015U3_FIXED_RTTI  // msvc cl.exe >= v19.00.24210 (msvc2015u3) employs some fixes
#  endif

#endif


#endif  // include guard


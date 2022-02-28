#ifndef CPPSTD_HEADERS_REDIRECT_H_
#define CPPSTD_HEADERS_REDIRECT_H_


#if defined (DDK_VER)  //  ddk/wdk toolset

#  if    (DDK_VER < 6000)    //  ddk2600 ddk3790
#    define NTKE_INCLUDE_DDK_STL_PATH    ../crt
#    define NTKE_INCLUDE_DDK_CPPRT_PATH  ../crt
#  elif  (DDK_VER < 9000)    //  wdk6001 wdk7600
#    define NTKE_INCLUDE_DDK_STL_PATH    ../api/crt/stl70
#    define NTKE_INCLUDE_DDK_CPPRT_PATH  ../crt
#  elif  (DDK_VER < 10000)   //  wdk9200 wdk9600
#    define NTKE_INCLUDE_DDK_STL_PATH    ../include
#    define NTKE_INCLUDE_DDK_CPPRT_PATH  ../include
#  elif  (DDK_VER >= 10240)  //  wdk10240 ewdk*
#    define NTKE_INCLUDE_DDK_STL_PATH    ../include
#    define NTKE_INCLUDE_DDK_CPPRT_PATH  ../include
#  else
#    error unsupported DDK_VER
#  endif

#else  //  msvc toolset

#  define NTKE_INCLUDE_DDK_STL_PATH    ../include
#  define NTKE_INCLUDE_DDK_CPPRT_PATH  ../include

#endif


#define NTKE_INCLUDE_DDK_STL(header)    _STLP_MAKE_HEADER(NTKE_INCLUDE_DDK_STL_PATH,header)
#define NTKE_INCLUDE_DDK_CPPRT(header)  _STLP_MAKE_HEADER(NTKE_INCLUDE_DDK_CPPRT_PATH,header)


#endif  // include guard


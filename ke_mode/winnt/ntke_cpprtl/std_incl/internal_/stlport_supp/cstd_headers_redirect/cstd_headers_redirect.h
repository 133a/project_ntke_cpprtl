#ifndef CSTD_HEADERS_REDIRECT_H_
#define CSTD_HEADERS_REDIRECT_H_


// this redirection is required for msvc2015-based and newer toolsets
// due to CRT C-std include files are splitted into separate locations (legacy vs ucrt)


#if defined (DDK_VER)  // ddk/wdk toolset

#  if  (DDK_VER >= 10240)  // wdk10240 ewdk*
#    define NTKE_INCLUDE_DDK_CRT_PATH    ../include
#    define NTKE_INCLUDE_DDK_UCRT_PATH   ../ucrt
#  else
#    error unsupported DDK_VER
#  endif

#else                  // msvc toolset

#  if  defined (_MSC_VER) && (_MSC_VER >= 1900)  // >=msvc2015
#    define NTKE_INCLUDE_DDK_CRT_PATH    ../include
#    define NTKE_INCLUDE_DDK_UCRT_PATH   ../ucrt
#  else
#    error unsupported _MSC_VER
#  endif

#endif


#define NTKE_INCLUDE_DDK_CRT(header)    _STLP_MAKE_HEADER(NTKE_INCLUDE_DDK_CRT_PATH,header)
#define NTKE_INCLUDE_DDK_UCRT(header)   _STLP_MAKE_HEADER(NTKE_INCLUDE_DDK_UCRT_PATH,header)


#endif  // include guard


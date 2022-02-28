//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef CHECK_CT_H_
#define CHECK_CT_H_


#define COMPILE_TIME_CHECK(EXPR, MSG)                                                \
  template <bool> struct _COMPILE_TIME_ERROR_##MSG;                                  \
  template <>     struct _COMPILE_TIME_ERROR_##MSG<true>  { typedef int result_t; }; \
  typedef _COMPILE_TIME_ERROR_##MSG<EXPR>::result_t  _COMPILE_TIME_CHECK_##MSG;


#endif  // include guard


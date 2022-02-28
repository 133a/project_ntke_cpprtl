//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTL_INIT_GSTATIC_H_
#define RTL_INIT_GSTATIC_H_


namespace cpprtl { namespace gstatic
{
  bool start();
  void stop();

  typedef void (__cdecl *pf_t)(void);
}}  // namespace cpprtl::gstatic


#endif  // include guard

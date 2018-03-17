/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTL_INIT_GSTATIC_H_
#define RTL_INIT_GSTATIC_H_


namespace cpprtl
{
namespace gstatic
{

  int   start();
  void  stop();

  typedef void (__cdecl *pf_t)(void);

}  // namespace gstatic
}  // namespace cpprtl


#endif  // include guard


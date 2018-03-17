/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_CONFIG_H_
#define RTTI_CONFIG_H_


#ifdef CFG_RVA
#undef CFG_RVA
#endif


#ifndef _MSC_VER
typedef unsigned int size_t;
#endif


namespace cpprtl
{
  namespace msvc_internal_data
  {
    typedef unsigned long int array_length_t;
  }
  typedef int  image_base_t;  // a fake placeholder
}


#endif  // include guard


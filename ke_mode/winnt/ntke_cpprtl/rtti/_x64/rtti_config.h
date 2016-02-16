/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_CONFIG_H_
#define RTTI_CONFIG_H_


#define CFG_RVA


#ifndef _MSC_VER
typedef unsigned long long  size_t;
#endif

namespace cpprtl
{
  namespace msvc_internal_data
  {
    typedef unsigned long  array_length_t;
  }

  typedef unsigned  rva_t;
  typedef ::size_t  image_base_t;
}


#endif // include guard


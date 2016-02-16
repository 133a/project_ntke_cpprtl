/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_CONFIG_H_
#define RTTI_CONFIG_H_


#ifdef CFG_RVA
#undef CFG_RVA
#endif


#ifndef _MSC_VER
typedef unsigned long  size_t;
#endif

namespace cpprtl
{
  namespace msvc_internal_data
  {
    typedef unsigned long  array_length_t;
  }

  typedef unsigned int   rva_t;
  typedef unsigned long  image_base_t;  //  actually it's a fake 'cos the RVA isn't used
}


#endif // include guard


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_CONFIG_H_
#define EH_CONFIG_H_


#include "eh_cfg_common.h"


#define CFG_EH_TABLE_BASED
#define CFG_RVA

#ifndef NTKE_EH_CONFIG_DO_NOT_USE_STACK_WALKER
#  define CFG_EH_STACK_WALKER
#endif



#ifndef _MSC_VER
typedef unsigned long long int size_t;
#endif

namespace cpprtl
{
  namespace msvc_internal_data
  {
    typedef unsigned int array_length_t;
  }

  typedef unsigned int   rva_t;
  typedef ::size_t       image_base_t;
}


#endif  // include guard


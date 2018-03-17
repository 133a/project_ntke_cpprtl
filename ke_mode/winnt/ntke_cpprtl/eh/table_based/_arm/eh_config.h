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


#ifndef NTKE_EH_CONFIG_DO_NOT_USE_STACK_WALKER  // TODO (_M_ARM && NT_KERNEL_MODE [nt6.2.9200]) : 'RtlRestoreContext()' doesn't support a stack consolidation ?
#  define CFG_EH_STACK_WALKER
#endif



#ifndef _MSC_VER
typedef unsigned long int size_t;
#endif

namespace cpprtl
{
  namespace msvc_internal_data
  {
    typedef unsigned int array_length_t;
  }

  typedef unsigned int   rva_t;
#ifdef NT_KERNEL_MODE
  typedef unsigned int   image_base_t;
#else
  typedef unsigned long  image_base_t;
#endif
}


#endif  // include guard


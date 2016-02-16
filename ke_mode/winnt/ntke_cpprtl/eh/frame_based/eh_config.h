/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_CONFIG_FRAME_BASED_H_
#define EH_CONFIG_FRAME_BASED_H_


#include "eh_cfg_common.h"


#define CFG_EH_FRAME_BASED


#ifndef NTKE_EH_CONFIG_DO_NOT_USE_STACK_WALKER
#define CFG_EH_STACK_WALKER
#endif


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
    typedef unsigned int array_length_t;
  }
}



#endif // include guard


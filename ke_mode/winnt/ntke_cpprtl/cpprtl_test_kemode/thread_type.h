/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef THREAD_TYPE_H_
#define THREAD_TYPE_H_


#ifdef  NTKE_KTHREAD
#  include <ntke/kthread.h>
#  include <type_aux/functee.h>
#  include <type_aux/ref_holder.h>
#else
#  include "aux_thread.h"
#endif


namespace aux_
{
#ifdef NTKE_KTHREAD  //  choose threads to be used
  typedef ntke::kthread thread_type;
#else
  typedef aux_::kthread thread_type;
#endif
}


#endif // include guard


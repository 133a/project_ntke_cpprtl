//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef SYNC_TYPE_H_
#define SYNC_TYPE_H_


#ifdef NTKE_LIB
#  include <ntke/fast_mutex.h>
#  include <ntke/kevent.h>
#  include <ntke/kmutex.h>
#  include <ntke/spin_lock.h>
#else
#  include "aux_sync.h"
#endif


namespace cpprtl_tests
{
  //  choose sync types to be used
#ifdef NTKE_LIB
  typedef ntke::kevent                          event_type;

  typedef ntke::spin_lock                       spin_lock_type;
  typedef ntke::spin_lock::auto_lock_type       auto_spin_lock_type;
  typedef ntke::spin_lock::dpc_auto_lock_type   dpc_auto_spin_lock_type;

  typedef ntke::kmutex                          mutex_type;
  typedef ntke::kmutex::auto_lock_type          auto_mutex_type;

  typedef ntke::fast_mutex                      fast_mutex_type;
  typedef ntke::fast_mutex::auto_lock_type      auto_fast_mutex_type;
  typedef ntke::fast_mutex::apc_auto_lock_type  apc_auto_fast_mutex_type;
#else
  typedef aux_::kevent               event_type;

  typedef aux_::spin_lock            spin_lock_type;
  typedef aux_::auto_spin_lock       auto_spin_lock_type;
  typedef aux_::dpc_auto_spin_lock   dpc_auto_spin_lock_type;

  typedef aux_::kmutex               mutex_type;
  typedef aux_::auto_mutex           auto_mutex_type;

  typedef aux_::fast_mutex           fast_mutex_type;
  typedef aux_::auto_fast_mutex      auto_fast_mutex_type;
  typedef aux_::apc_auto_fast_mutex  apc_auto_fast_mutex_type;
#endif

}  // namespace cpprtl_tests


#endif  // include guard


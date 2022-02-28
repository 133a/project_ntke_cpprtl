//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtl_framework.h"
#include "rtl_init_heap.h"
#include "rtl_init_gstatic.h"


namespace
{
  int seq_idx;

  typedef bool (*start_ft) ();
  typedef void (*stop_ft)  ();

  start_ft const start_seq[] = 
  {
    cpprtl::heap::start
  , cpprtl::gstatic::start
  };

  stop_ft const stop_seq[] = 
  {
    cpprtl::heap::stop
  , cpprtl::gstatic::stop
  };
}  // namespace

namespace cpprtl
{
  bool start()
  {
    ASSERT ( sizeof(start_seq) == sizeof(stop_seq) );
    bool status = true;
    for
    (
      seq_idx = 0
    ; seq_idx < sizeof(start_seq)/sizeof(start_seq[0]) && status
    ; ++seq_idx
    )
    {
      status = start_seq[seq_idx]();
    }
    return status;
  }

  void stop()
  {
    while ( --seq_idx >= 0 )
    {
      stop_seq[seq_idx]();
    }
  }
}  // namespace cpprtl

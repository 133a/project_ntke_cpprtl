/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtl_framework_specific_header.h"

#include "rtl_init_heap.h"
#include "rtl_init_gstatic.h"


/////////////////////////////////////////////////////////////////////////////
////  RTL init/exit
/////////////////////////////////////////////////////////////////////////////

namespace
{
  int seq_idx = 0;

  typedef int   (*start_ft) ();
  typedef void  (*stop_ft)  ();

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

  int start()
  {
    ASSERT ( sizeof(start_seq) == sizeof(stop_seq) );
    int status = STATUS_SUCCESS;
    for
    (
      seq_idx = 0
    ; seq_idx < sizeof(start_seq)/sizeof(start_seq[0]) && NT_SUCCESS(status)
    ; ++seq_idx
    )
    {
      status = start_seq[seq_idx]();
    }
    return status;
  }


  void stop()
  {
    for ( ; --seq_idx >= 0 ; --seq_idx )
    {
      stop_seq[seq_idx]();
    }
  }

}  // namespace cpprtl


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef KALLOC_H_
#define KALLOC_H_


#include "test_context.h"
#include <utility>


#ifdef NT_KERNEL_MODE
  #include <ntke/kallocator.h>
  namespace aux_
  {
    typedef ntke::nonpaged_allocator <test_object<int> >  int_obj_nonpaged_allocator;
    typedef ntke::paged_allocator    <test_object<int> >  int_obj_paged_allocator;

    typedef ntke::nonpaged_allocator <std::pair<int, test_object<int> > >  int_obj_pair_nonpaged_allocator;
    typedef ntke::paged_allocator    <std::pair<int, test_object<int> > >  int_obj_pair_paged_allocator;
  }
#else
  #include <memory>
  namespace aux_
  {
    typedef std::allocator <test_object<int> >  int_obj_nonpaged_allocator;
    typedef std::allocator <test_object<int> >  int_obj_paged_allocator;

    typedef std::allocator <std::pair<int, test_object<int> > >  int_obj_pair_nonpaged_allocator;
    typedef std::allocator <std::pair<int, test_object<int> > >  int_obj_pair_paged_allocator;
  } 
#endif


#endif  // include guard


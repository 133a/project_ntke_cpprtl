/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


#if defined ( _MSC_VER )  &&  ( _MSC_VER < 1400 )  &&  defined ( _M_IX86 )
   // '__thiscall' is considered reserved name but replaced by '__stdcall' if used in source
   // using warn-level 2 to see it in cl output for being aware of the things may go wrong
#  define EHVEC_BY_ASM_THUNK 1
#  pragma warning(2:4234)
#endif


namespace
{
#if ( _MSC_VER >= 1900 )  // msvc2015
  typedef ::size_t  size_type;
  typedef ::size_t  index_type;
#else
  typedef ::size_t  size_type;
  typedef int       index_type;
#endif
}


#ifdef EHVEC_BY_ASM_THUNK
namespace
{
  struct cl13_obj {};

  typedef void (cl13_obj::*dtor_ft)();
  typedef void (cl13_obj::*ctor_ft)();
  typedef void (cl13_obj::*ctor_vb_ft)(int);
  typedef void (cl13_obj::*cctor_ft)(void*);
  typedef void (cl13_obj::*cctor_vb_ft)(void*, int);


  inline void dtor_thunk(dtor_ft dtor, void* this_ptr)
  {
    (reinterpret_cast<cl13_obj*>(this_ptr)->*dtor)();
  }

  inline void ctor_thunk(ctor_ft ctor, void* this_ptr)
  {
    (reinterpret_cast<cl13_obj*>(this_ptr)->*ctor)();
  }

  inline void ctor_vb_thunk(ctor_ft ctor_vb, void* this_ptr)
  {
    (reinterpret_cast<cl13_obj*>(this_ptr)->*reinterpret_cast<ctor_vb_ft>(ctor_vb))(1);  // see the comment at the corresponding entry point
  }

  inline void cctor_thunk(cctor_ft cctor, void* this_ptr, void* that_ptr)
  {
    (reinterpret_cast<cl13_obj*>(this_ptr)->*cctor)(that_ptr);
  }

  inline void cctor_vb_thunk(cctor_ft cctor_vb, void* this_ptr, void* that_ptr)
  {
    (reinterpret_cast<cl13_obj*>(this_ptr)->*reinterpret_cast<cctor_vb_ft>(cctor_vb))(that_ptr, 1);  // see the comment at the corresponding entry point
  }
}  // namespace
#else
namespace
{
  typedef void (__thiscall *dtor_ft)(void*);
  typedef void (__thiscall *ctor_ft)(void*);
  typedef void (__thiscall *ctor_vb_ft)(void*, int);
  typedef void (__thiscall *cctor_ft)(void*, void*);
  typedef void (__thiscall *cctor_vb_ft)(void*, void*, int);


  inline void dtor_thunk(dtor_ft dtor, void* this_ptr)
  {
    dtor(this_ptr);
  }

  inline void ctor_thunk(ctor_ft ctor, void* this_ptr)
  {
    ctor(this_ptr);
  }

  inline void ctor_vb_thunk(ctor_ft ctor_vb, void* this_ptr)
  {
    (reinterpret_cast<ctor_vb_ft>(ctor_vb))(this_ptr, 1);  // see the comment at the corresponding entry point
  }

  inline void cctor_thunk(cctor_ft cctor, void* this_ptr, void* that_ptr)
  {
    cctor(this_ptr, that_ptr);
  }

  inline void cctor_vb_thunk(cctor_ft cctor_vb, void* this_ptr, void* that_ptr)
  {
    (reinterpret_cast<cctor_vb_ft>(cctor_vb))(this_ptr, that_ptr, 1);  // see the comment at the corresponding entry point
  }
}  // namespace
#endif  // EHVEC_BY_ASM_THUNK


namespace
{

  class array_iterator_base
  {
  protected:
    void*       const&  array;
    size_type   const&  obj_size;
    index_type  const&  arr_size;
    index_type          current;

    array_iterator_base
    (
      void*       const& arr
    , size_type   const& obj_sz
    , index_type  const& arr_sz
    )
      : array    ( arr )
      , obj_size ( obj_sz )
      , arr_size ( arr_sz )
      , current  ( -1 )
    {}

  };


  class fwd_array_iterator
    : public array_iterator_base
  {
  public:
    fwd_array_iterator
    (
      void*       const& arr
    , size_type   const& obj_sz
    , index_type  const& arr_sz
    )
      : array_iterator_base ( arr, obj_sz, arr_sz )
    {
      reset();
    }

    void reset()
    {
      current = 0;  // valid values [1:arr_size]
    }

    bool next()
    {
      return ++current <= arr_size;
    }

    bool prev()
    {
      return --current > 0;
    }

    void* get() const
    {
      return reinterpret_cast<void*>(reinterpret_cast<size_type>(array) + (current - 1) * obj_size);
    }

  };


  class rev_array_iterator
    : public array_iterator_base
  {
  public:
    rev_array_iterator
    (
      void*       const& arr
    , size_type   const& obj_sz
    , index_type  const& arr_sz
    )
      : array_iterator_base ( arr, obj_sz, arr_sz )
    {
      reset();
    }

    void reset()
    {
      current = arr_size + 1;  // valid values [1:arr_size]
    }

    bool next()
    {
      return --current > 0;
    }

    bool prev()
    {
      return ++current <= arr_size;
    }

    void* get() const
    {
      return reinterpret_cast<void*>(reinterpret_cast<size_type>(array) + (arr_size - current) * obj_size);
    }

  };


  inline void throw_incorrect_array_unwind()
  {
    cpprtl::eh::aux_::invalid_exception
    (
      cpprtl::eh::EXCEPTION_CODE_CPP
    , cpprtl::eh::EXCEPTION_SUBCODE_ARRAY_UNWIND_DTOR_THROW
    );
  }

}  // namespace


////////////////////////////////////////////////////
////////////////////////////////////////////////////
////  compiler's internally pre-declared EH entry points 
////  for array constructing/unwinding routines
////////////////////////////////////////////////////


////////////////////////////////////////////////////
// `eh vector destructor iterator'

#ifdef EHVEC_BY_ASM_THUNK
  extern "C" void __stdcall ehvec_dtor_impl
#else
  void __stdcall __ehvec_dtor
#endif
(
  void*       array
, size_type   obj_size
, index_type  arr_size
, dtor_ft     dtor
)
{
  rev_array_iterator arr_iter(array, obj_size, arr_size);
  try
  {
    while ( arr_iter.next() )
    {
      dtor_thunk(dtor, arr_iter.get());
    }
  }
  catch (...)
  {
    try
    {
      while ( arr_iter.next() )  // skip the throwed array element
      {
        dtor_thunk(dtor, arr_iter.get());
      }
    }
    catch (...)
    {
      throw_incorrect_array_unwind();
    }
    throw;
  }
}


////////////////////////////////////////////////////
// `eh vector constructor iterator'

#ifdef EHVEC_BY_ASM_THUNK
  extern "C" void __stdcall ehvec_ctor_impl
#else
  void __stdcall __ehvec_ctor
#endif
(
  void*       array
, size_type   obj_size
, index_type  arr_size
, ctor_ft     ctor
, dtor_ft     dtor
)
{
  fwd_array_iterator arr_iter(array, obj_size, arr_size);
  try
  {
    while ( arr_iter.next() )
    {
      ctor_thunk(ctor, arr_iter.get());
    }
  }
  catch (...)
  {
    try
    {
      while ( arr_iter.prev() )
      {
        dtor_thunk(dtor, arr_iter.get());
      }
    }
    catch (...)
    {
      throw_incorrect_array_unwind();
    }
    throw;
  }
}


////////////////////////////////////////////////////
// `eh vector vbase constructor iterator'

#ifdef EHVEC_BY_ASM_THUNK
  extern "C" void __stdcall ehvec_ctor_vb_impl
#else
  void __stdcall __ehvec_ctor_vb
#endif
  // note the type 'ctor_ft' instead of 'ctor_vb_ft' - it is to match the internal msvc type of this entry point, 
  // but the actual type is 'ctor_vb_ft', so we have to cast in the xtor_vb_thunk()
(
  void*       array
, size_type   obj_size
, index_type  arr_size
, ctor_ft     ctor_vb
, dtor_ft     dtor
)
{
  fwd_array_iterator arr_iter(array, obj_size, arr_size);
  try
  {
    while ( arr_iter.next() )
    {
      ctor_vb_thunk(ctor_vb, arr_iter.get());
    }
  }
  catch (...)
  {
    try
    {
      while ( arr_iter.prev() )
      {
        dtor_thunk(dtor, arr_iter.get());
      }
    }
    catch (...)
    {
      throw_incorrect_array_unwind();
    }
    throw;
  }
}


////////////////////////////////////////////////////
// `eh vector copy constructor iterator'

#ifdef EHVEC_BY_ASM_THUNK
  extern "C" void __stdcall ehvec_copy_ctor_impl
#else
  void __stdcall __ehvec_copy_ctor
#endif
(
  void*       dst_array
, void*       src_array
, size_type   obj_size
, index_type  arr_size
, cctor_ft    cctor
, dtor_ft     dtor
)
{
  fwd_array_iterator dst_iter(dst_array, obj_size, arr_size);
  try
  {
    for
    (
      fwd_array_iterator src_iter(src_array, obj_size, arr_size)
    ; dst_iter.next() , src_iter.next()
    ;
    )
    {
      cctor_thunk(cctor, dst_iter.get(), src_iter.get());
    }
  }
  catch (...)
  {
    try
    {
      while ( dst_iter.prev() )
      {
        dtor_thunk(dtor, dst_iter.get());
      }
    }
    catch (...)
    {
      throw_incorrect_array_unwind();
    }
    throw;
  }
}


////////////////////////////////////////////////////
// `eh vector vbase copy constructor iterator'

#ifdef EHVEC_BY_ASM_THUNK
  extern "C" void __stdcall ehvec_copy_ctor_vb_impl
#else
  void __stdcall __ehvec_copy_ctor_vb
#endif
  // note the type 'cctor_ft' instead of 'cctor_vb_ft' - it is to match the internal msvc type of this entry point, 
  // but the actual type is 'cctor_vb_ft', so we have to cast in the xtor_vb_thunk()
(
  void*       dst_array
, void*       src_array
, size_type   obj_size
, index_type  arr_size
, cctor_ft    cctor_vb
, dtor_ft     dtor
)
{
  fwd_array_iterator dst_iter(dst_array, obj_size, arr_size);
  try
  {
    for
    (
      fwd_array_iterator src_iter(src_array, obj_size, arr_size)
    ; dst_iter.next() , src_iter.next()
    ; 
    )
    {
      cctor_vb_thunk(cctor_vb, dst_iter.get(), src_iter.get());
    }
  }
  catch (...)
  {
    try
    {
      while ( dst_iter.prev() )
      {
        dtor_thunk(dtor, dst_iter.get());
      }
    }
    catch (...)
    {
      throw_incorrect_array_unwind();
    }
    throw;
  }
}


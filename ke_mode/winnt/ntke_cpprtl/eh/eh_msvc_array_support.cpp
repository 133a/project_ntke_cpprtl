/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


#if defined ( _MSC_VER )  &&  ( _MSC_VER < 1400 )  &&  defined ( _M_IX86 )
   // '__thiscall' is considered reserved name but replaced by '__stdcall' when used.
#  define EHVEC_BY_ASM_THUNK 1
   // using warn-level 2 for being aware of the things may go wrong
#  pragma warning(2:4234)
#endif


namespace
{
#if ( _MSC_VER >= 1900 )  // >=msvc2015
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
    void*      const  array;
    size_type  const  obj_size;
    size_type  const  array_len;
    size_type         current;

    array_iterator_base
    (
      void*      const&  arr
    , size_type  const&  obj_sz
    , size_type  const&  arr_len
    , size_type  const&  cur
    )
      : array     ( arr )
      , obj_size  ( obj_sz )
      , array_len ( arr_len )
      , current   ( cur )
    {}

  public:
    bool valid() const
    {
      return current < array_len;
    }

    void* get() const
    {
      return reinterpret_cast<void*>(reinterpret_cast<size_type>(array) + current * obj_size);
    }
  };


  class fwd_array_iterator
    : public array_iterator_base
  {
  public:
    fwd_array_iterator
    (
      void*      const&  arr
    , size_type  const&  obj_sz
    , size_type  const&  arr_len
    )
      : array_iterator_base ( arr, obj_sz, arr_len, 0 )
    {}

    void next()
    {
      ++current;
    }

    void prev()
    {
      --current;
    }
  };


  class rev_array_iterator
    : public array_iterator_base
  {
  public:
    rev_array_iterator
    (
      void*      const&  arr
    , size_type  const&  obj_sz
    , size_type  const&  arr_len
    )
      : array_iterator_base ( arr, obj_sz, arr_len, arr_len - 1 )
    {}

    void next()
    {
      --current;
    }

    void prev()
    {
      ++current;
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
, index_type  array_len
, dtor_ft     dtor
)
{
  rev_array_iterator arr(array, obj_size, array_len);
  try
  {
    for ( ; arr.valid() ; arr.next() )
    {
      dtor_thunk(dtor, arr.get());
    }
  }
  catch (...)
  {
    try
    {
      for ( arr.next() ; arr.valid() ; arr.next() )  // skip the throwed array element
      {
        dtor_thunk(dtor, arr.get());
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
, index_type  array_len
, ctor_ft     ctor
, dtor_ft     dtor
)
{
  fwd_array_iterator arr(array, obj_size, array_len);
  try
  {
    for ( ; arr.valid() ; arr.next() )
    {
      ctor_thunk(ctor, arr.get());
    }
  }
  catch (...)
  {
    try
    {
      for ( arr.prev() ; arr.valid() ; arr.prev() )  // skip the throwed array element
      {
        dtor_thunk(dtor, arr.get());
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
, index_type  array_len
, ctor_ft     ctor_vb
, dtor_ft     dtor
)
{
  fwd_array_iterator arr(array, obj_size, array_len);
  try
  {
    for ( ; arr.valid() ; arr.next() )
    {
      ctor_vb_thunk(ctor_vb, arr.get());
    }
  }
  catch (...)
  {
    try
    {
      for ( arr.prev() ; arr.valid() ; arr.prev() )  // skip the throwed array element
      {
        dtor_thunk(dtor, arr.get());
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
, index_type  array_len
, cctor_ft    cctor
, dtor_ft     dtor
)
{
  fwd_array_iterator dst(dst_array, obj_size, array_len);
  try
  {
    for
    (
      fwd_array_iterator src(src_array, obj_size, array_len)
    ; dst.valid()
    ; dst.next() , src.next()
    )
    {
      cctor_thunk(cctor, dst.get(), src.get());
    }
  }
  catch (...)
  {
    try
    {
      for ( dst.prev() ; dst.valid() ; dst.prev() )  // skip the throwed array element
      {
        dtor_thunk(dtor, dst.get());
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
, index_type  array_len
, cctor_ft    cctor_vb
, dtor_ft     dtor
)
{
  fwd_array_iterator dst(dst_array, obj_size, array_len);
  try
  {
    for
    (
      fwd_array_iterator src(src_array, obj_size, array_len)
    ; dst.valid()
    ; dst.next() , src.next()
    )
    {
      cctor_vb_thunk(cctor_vb, dst.get(), src.get());
    }
  }
  catch (...)
  {
    try
    {
      for ( dst.prev() ; dst.valid() ; dst.prev() )  // skip the throwed array element
      {
        dtor_thunk(dtor, dst.get());
      }
    }
    catch (...)
    {
      throw_incorrect_array_unwind();
    }
    throw;
  }
}


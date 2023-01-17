//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_aux.h"
#include "eh_exception_code.h"


#if defined (_MSC_VER) && (_MSC_VER < 1400) && defined(_M_IX86)  // < msvc2005_x86
  // '__thiscall' is considered reserved name but replaced by '__stdcall' when used
  #define CPPRTL_EH_CFG_EHVEC_THUNK 1
  // using warn-level 2 for being aware of the things may go wrong
  #pragma warning(error:4234)
#endif

namespace
{
#if (_MSC_VER >= 1900)  // >=msvc2015
  typedef size_t size_type;
  typedef size_t index_type;
#else
  typedef size_t size_type;
  typedef int    index_type;
#endif
}

#ifdef CPPRTL_EH_CFG_EHVEC_THUNK
namespace
{
  struct object_type {};
  typedef void (object_type::*dtor_ft)();
  typedef void (object_type::*ctor_ft)();
  typedef void (object_type::*ctor_vb_ft)(int);
  typedef void (object_type::*cctor_ft)(void*);
  typedef void (object_type::*cctor_vb_ft)(void*, int);

  void dtor_thunk(dtor_ft dtor, void* this_ptr)
  {
    (reinterpret_cast<object_type*>(this_ptr)->*dtor)();
  }

  void ctor_thunk(ctor_ft ctor, void* this_ptr)
  {
    (reinterpret_cast<object_type*>(this_ptr)->*ctor)();
  }

  void ctor_vb_thunk(ctor_ft ctor_vb, void* this_ptr)
  {
    (reinterpret_cast<object_type*>(this_ptr)->*reinterpret_cast<ctor_vb_ft>(ctor_vb))(1);  // see the comment at the corresponding entry point
  }

  void cctor_thunk(cctor_ft cctor, void* this_ptr, void* that_ptr)
  {
    (reinterpret_cast<object_type*>(this_ptr)->*cctor)(that_ptr);
  }

  void cctor_vb_thunk(cctor_ft cctor_vb, void* this_ptr, void* that_ptr)
  {
    (reinterpret_cast<object_type*>(this_ptr)->*reinterpret_cast<cctor_vb_ft>(cctor_vb))(that_ptr, 1);  // see the comment at the corresponding entry point
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

  void dtor_thunk(dtor_ft dtor, void* this_ptr)
  {
    dtor(this_ptr);
  }

  void ctor_thunk(ctor_ft ctor, void* this_ptr)
  {
    ctor(this_ptr);
  }

  void ctor_vb_thunk(ctor_ft ctor_vb, void* this_ptr)
  {
    (reinterpret_cast<ctor_vb_ft>(ctor_vb))(this_ptr, 1);  // see the comment at the corresponding entry point
  }

  void cctor_thunk(cctor_ft cctor, void* this_ptr, void* that_ptr)
  {
    cctor(this_ptr, that_ptr);
  }

  void cctor_vb_thunk(cctor_ft cctor_vb, void* this_ptr, void* that_ptr)
  {
    (reinterpret_cast<cctor_vb_ft>(cctor_vb))(this_ptr, that_ptr, 1);  // see the comment at the corresponding entry point
  }
}  // namespace
#endif  // CPPRTL_EH_CFG_EHVEC_THUNK

namespace
{
  class array_iterator_base
  {
  protected:
    void*     const array;
    size_type const obj_size;
    size_type const array_len;
    size_type       current;

    array_iterator_base
    (
      void*     const arr
    , size_type const obj_sz
    , size_type const arr_len
    , size_type const cur
    )
      : array     ( arr )
      , obj_size  ( obj_sz )
      , array_len ( arr_len )
      , current   ( cur )
    {}

  public:
    bool valid() const { return current < array_len; }
    void* element() const { return reinterpret_cast<void*>(reinterpret_cast<size_type>(array) + current * obj_size); }
  };

  class fwd_array_iterator
    : public array_iterator_base
  {
  public:
    fwd_array_iterator
    (
      void*     const arr
    , size_type const obj_sz
    , size_type const arr_len
    )
      : array_iterator_base ( arr, obj_sz, arr_len, 0 )
    {}

    void next() { ++current; }
    void prev() { --current; }
  };

  class rev_array_iterator
    : public array_iterator_base
  {
  public:
    rev_array_iterator
    (
      void*     const arr
    , size_type const obj_sz
    , size_type const arr_len
    )
      : array_iterator_base ( arr, obj_sz, arr_len, arr_len - 1 )
    {}

    void next() { --current; }
    void prev() { ++current; }
  };

  void terminate_array_unwind()
  {
    cpprtl::eh::aux_::terminate
    (
      cpprtl::eh::EXCEPTION_CODE_CPP
    , cpprtl::eh::EXCEPTION_SUBCODE_ARRAY_UNWIND_DTOR_THROW
    );
  }
}  // namespace


//================================
// `eh vector destructor iterator'
//--------------------------------
#ifdef CPPRTL_EH_CFG_EHVEC_THUNK
  extern "C" void __stdcall ehvec_dtor_impl
#else
  void __stdcall __ehvec_dtor
#endif
(
  void*       arr
, size_type   obj_size
, index_type  arr_len
, dtor_ft     dtor
)
{
  for ( rev_array_iterator array(arr, obj_size, arr_len) ; array.valid() ; array.next() )
  {
    try
    {
      dtor_thunk(dtor, array.element());
    }
    catch (...)
    {
      for ( array.next() ; array.valid() ; array.next() )  // skip the thrown array element
      {
        try
        {
          dtor_thunk(dtor, array.element());
        }
        catch (...)
        {
          terminate_array_unwind();
        }
      }
      throw;
    }
  }
}

//=================================
// `eh vector constructor iterator'
//---------------------------------
#ifdef CPPRTL_EH_CFG_EHVEC_THUNK
  extern "C" void __stdcall ehvec_ctor_impl
#else
  void __stdcall __ehvec_ctor
#endif
(
  void*       arr
, size_type   obj_size
, index_type  arr_len
, ctor_ft     ctor
, dtor_ft     dtor
)
{
  for ( fwd_array_iterator array(arr, obj_size, arr_len) ; array.valid() ; array.next() )
  {
    try
    {
      ctor_thunk(ctor, array.element());
    }
    catch (...)
    {
      for ( array.prev() ; array.valid() ; array.prev() )  // skip the thrown array element
      {
        try
        {
          dtor_thunk(dtor, array.element());
        }
        catch (...)
        {
          terminate_array_unwind();
        }
      }
      throw;
    }
  }
}

//=======================================
// `eh vector vbase constructor iterator'
//---------------------------------------
#ifdef CPPRTL_EH_CFG_EHVEC_THUNK
  extern "C" void __stdcall ehvec_ctor_vb_impl
#else
  void __stdcall __ehvec_ctor_vb
#endif
  // note the type 'ctor_ft' instead of 'ctor_vb_ft' - it is to match the internal msvc type of this entry point, 
  // but the actual type is 'ctor_vb_ft', so we have to cast in the ctor_vb_thunk()
(
  void*       arr
, size_type   obj_size
, index_type  arr_len
, ctor_ft     ctor_vb
, dtor_ft     dtor
)
{
  for ( fwd_array_iterator array(arr, obj_size, arr_len) ; array.valid() ; array.next() )
  {
    try
    {
      ctor_vb_thunk(ctor_vb, array.element());
    }
    catch (...)
    {
      for ( array.prev() ; array.valid() ; array.prev() )  // skip the thrown array element
      {
        try
        {
          dtor_thunk(dtor, array.element());
        }
        catch (...)
        {
          terminate_array_unwind();
        }
      }
      throw;
    }
  }
}

//======================================
// `eh vector copy constructor iterator'
//--------------------------------------
#ifdef CPPRTL_EH_CFG_EHVEC_THUNK
  extern "C" void __stdcall ehvec_copy_ctor_impl
#else
  void __stdcall __ehvec_copy_ctor
#endif
(
  void*       dst
, void*       src
, size_type   obj_size
, index_type  arr_len
, cctor_ft    cctor
, dtor_ft     dtor
)
{
  for
  (
    fwd_array_iterator src_array(src, obj_size, arr_len), dst_array(dst, obj_size, arr_len)
  ; src_array.valid()
  ; src_array.next(), dst_array.next()
  )
  {
    try
    {
      cctor_thunk(cctor, dst_array.element(), src_array.element());
    }
    catch (...)
    {
      for ( dst_array.prev() ; dst_array.valid() ; dst_array.prev() )  // skip the thrown destination array element
      {
        try
        {
          dtor_thunk(dtor, dst_array.element());
        }
        catch (...)
        {
          terminate_array_unwind();
        }
      }
      throw;
    }
  }
}

//============================================
// `eh vector vbase copy constructor iterator'
//--------------------------------------------
#ifdef CPPRTL_EH_CFG_EHVEC_THUNK
  extern "C" void __stdcall ehvec_copy_ctor_vb_impl
#else
  void __stdcall __ehvec_copy_ctor_vb
#endif
  // note the type 'cctor_ft' instead of 'cctor_vb_ft' - it is to match the internal msvc type of this entry point, 
  // but the actual type is 'cctor_vb_ft', so we have to cast in cctor_vb_thunk()
(
  void*       dst
, void*       src
, size_type   obj_size
, index_type  arr_len
, cctor_ft    cctor_vb
, dtor_ft     dtor
)
{
  for
  (
    fwd_array_iterator src_array(src, obj_size, arr_len), dst_array(dst, obj_size, arr_len)
  ; src_array.valid()
  ; src_array.next(), dst_array.next()
  )
  {
    try
    {
      cctor_vb_thunk(cctor_vb, dst_array.element(), src_array.element());
    }
    catch (...)
    {
      for ( dst_array.prev() ; dst_array.valid() ; dst_array.prev() )  // skip the thrown destination array element
      {
        try
        {
          dtor_thunk(dtor, dst_array.element());
        }
        catch (...)
        {
          terminate_array_unwind();
        }
      }
      throw;
    }
  }
}

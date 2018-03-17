/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef NTKE_KALLOCATOR_H_
#define NTKE_KALLOCATOR_H_


#if defined (_MSC_VER) && (_MSC_VER <= 1300)
#  define NO_TEMPLATE_PARTIAL_SPECIALIZATION 1  // get the ddk2600 compiler unfool
#endif


#include "../../rtl_framework_specific_header.h"
#include <cstddef>
#include <limits>
#include <new>


#if defined (max)
#  undef max  // neutralize if this annoyer has penetrated into the scope
#endif


namespace ntke
{
namespace internal_
{

  template <typename T, POOL_TYPE PT>
  class kallocator_base
  {
  public:
    typedef std::size_t        size_type;
    typedef std::ptrdiff_t     difference_type;
    typedef T*                 pointer;
    typedef T const*           const_pointer;
    typedef T&                 reference;
    typedef T const&           const_reference;
    typedef T                  value_type;

    pointer allocate(size_type const n, typename kallocator_base<void, PT>::const_pointer = 0)
    {
      if ( n > max_size() )
      {
        throw std::bad_alloc();
      }
      if ( n )
      {
        return static_cast<pointer>(::operator new(n * sizeof(value_type), PT));
      }
      return 0;
    }

    void construct(pointer p, const_reference val)
    {
      new(static_cast<void*>(p)) value_type(val);
    }

    void destroy(pointer p)
    {
      p->~value_type();
    }

    void deallocate(pointer p, size_type n)
    {
      ::operator delete(static_cast<void*>(p));
    }

    pointer address(reference val) const
    {
      return &val;
    }

    const_pointer address(const_reference val) const
    {
      return &val;
    }

    size_type max_size() const throw()
    {
      return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

  protected:

    kallocator_base() throw()  {}
    kallocator_base(kallocator_base const&) throw()  {}

    template <typename U>
    kallocator_base(kallocator_base<U, PT> const&) throw()  {}

    ~kallocator_base() throw()  {}

  };



#ifndef NO_TEMPLATE_PARTIAL_SPECIALIZATION

  template <POOL_TYPE PT>
  class kallocator_base<void, PT>
  {
  public:
    typedef void*         pointer;
    typedef void const*   const_pointer;
    typedef void          value_type;
  };

#else

  template <>
  class kallocator_base<void, NonPagedPool>
  {
  public:
    typedef void*         pointer;
    typedef void const*   const_pointer;
    typedef void          value_type;
  };

  template <>
  class kallocator_base<void, PagedPool>
  {
  public:
    typedef void*         pointer;
    typedef void const*   const_pointer;
    typedef void          value_type;
  };

#endif


}  // namespace internal_
}  // namespace ntke



namespace ntke
{

////  paged pool allocator

  template <typename T>
  class paged_allocator
    : public internal_::kallocator_base<T, PagedPool>
  {
  public:
    template <typename U>
    struct rebind
    {
      typedef paged_allocator<U> other;
    };


    paged_allocator() throw()  {}
    paged_allocator(paged_allocator const&) throw()  {}

    template <typename U>
    paged_allocator(paged_allocator<U> const&) throw()  {}

    ~paged_allocator() throw()  {}

  };


  template <>
  class paged_allocator<void>
    : public internal_::kallocator_base<void, PagedPool>
  {
  public:
    template <typename U>
    struct rebind
    {
      typedef paged_allocator<U> other;
    };
  };


  template <typename T1, typename T2>
  bool operator == (paged_allocator<T1> const&, paged_allocator<T2> const&)
  {
    return true;
  }

  template <typename T1, typename T2>
  bool operator != (paged_allocator<T1> const&, paged_allocator<T2> const&)
  {
    return false;
  }


////  nonpaged pool allocator
  template <typename T>
  class nonpaged_allocator
    : public internal_::kallocator_base<T, NonPagedPool>
  {
  public:
    template <typename U>
    struct rebind
    {
      typedef nonpaged_allocator<U> other;
    };


    nonpaged_allocator() throw()  {}
    nonpaged_allocator(nonpaged_allocator const&) throw()  {}

    template <typename U>
    nonpaged_allocator(nonpaged_allocator<U> const&) throw()  {}

    ~nonpaged_allocator() throw()  {}

  };


  template <>
  class nonpaged_allocator<void>
    : public internal_::kallocator_base<void, NonPagedPool>
  {
  public:
    template <typename U>
    struct rebind
    {
      typedef nonpaged_allocator<U> other;
    };
  };


  template <typename T1, typename T2>
  bool operator == (nonpaged_allocator<T1> const&, nonpaged_allocator<T2> const&)
  {
    return true;
  }

  template <typename T1, typename T2>
  bool operator != (nonpaged_allocator<T1> const&, nonpaged_allocator<T2> const&)
  {
    return false;
  }


}  // namespace ntke


#endif  // include guard


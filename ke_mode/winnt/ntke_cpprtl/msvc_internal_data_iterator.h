/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef MSVC_INTERNAL_DATA_ITERATOR_H_
#define MSVC_INTERNAL_DATA_ITERATOR_H_


//// ! keep the following classes' destructors trivial because of them being widely used in the seh-guarded frames

namespace cpprtl
{
namespace msvc_internal_data_aux
{


template <typename DST, typename A_SRC, typename SZ_SRC>
struct cl_info;


namespace aux_
{

  template <typename D, typename S, typename R = S>
  struct iterator_traits
  {
    typedef  D  DST;
    typedef  S  STOR;
    typedef  R  RET;
  };


//////////////////////////////////////////////////////////
//////////  msvc internal data iterator
//////////////////////////////////////////////////////////

  struct move_index_impl
  {
    template <typename ITER>
    static void incr(ITER& iter)
    {
      if (iter.idx < iter.array_size())
      {
        ++iter.idx;
      }
      else
      {
        iter.idx = 0;
      }
    }

    template <typename ITER>
    static void decr(ITER& iter)
    {
      if (iter.idx > 0)
      {
        --iter.idx;
      }
      else
      {
        iter.idx = iter.array_size();
      }
    }

    template <typename ITER>
    static void move(ITER& iter, ::size_t const& index)
    {
      iter.idx = index < iter.array_size() ? index : iter.array_size();
    }


    template <typename ITER>
    static void deface(ITER& iter)
    {
      iter.idx = iter.array_size();
    }
  };


  struct move_index_forward
  {
    template <typename ITER>
    static void init(ITER& iter)
    {
      reset(iter);
    }

    template <typename ITER>
    static void next(ITER& iter)
    {
      move_index_impl::incr(iter);
    }

    template <typename ITER>
    static void prev(ITER& iter)
    {
      move_index_impl::decr(iter);
    }

    template <typename ITER>
    static void move(ITER& iter, ::size_t const& index)
    {
      move_index_impl::move(iter, index);
    }

    template <typename ITER>
    static void reset(ITER& iter)
    {
      iter.idx = 0;
    }

    template <typename ITER>
    static void deface(ITER& iter)
    {
      move_index_impl::deface(iter);
    }
  };


  struct move_index_reverse
  {
    template <typename ITER>
    static void init(ITER& iter)
    {
      reset(iter);
    }

    template <typename ITER>
    static void next(ITER& iter)
    {
      move_index_impl::decr(iter);
    }

    template <typename ITER>
    static void prev(ITER& iter)
    {
      move_index_impl::incr(iter);
    }

    template <typename ITER>
    static void move(ITER& iter, ::size_t const& index)
    {
      move_index_impl::move(iter, index);
    }

    template <typename ITER>
    static void reset(ITER& iter)
    {
      iter.idx = iter.array_size() ? iter.array_size() - 1 : 0;
    }

    template <typename ITER>
    static void deface(ITER& iter)
    {
      move_index_impl::deface(iter);
    }
  };


  template <typename STOR, typename DIRECTION>
  class cl_iterator_storage
  {
    friend struct move_index_forward;
    friend struct move_index_reverse;
    friend struct move_index_impl;

  protected:
    STOR       item0;
    ::size_t   idx;
    ::size_t   arr_size;

  protected:
    cl_iterator_storage(STOR const& item0_, ::size_t const& arr_sz)
      : item0     ( item0_ )
      , idx       ( 0 )
      , arr_size  ( arr_sz )
    {
      DIRECTION::init(*this);
    }

    cl_iterator_storage(cl_iterator_storage const& iter)
      : idx       ( iter.idx )
      , arr_size  ( iter.arr_size )
      , item0     ( iter.item0 )
    {
    }

    template <typename DIR>
    cl_iterator_storage(cl_iterator_storage<STOR, DIR> const& iter)
      : idx       ( iter.index() )
      , arr_size  ( iter.array_size() )
      , item0     ( iter.get_item0() )
    {
    }

    cl_iterator_storage& operator=(cl_iterator_storage const& iter)
    {
      if (this != &iter)
      {
        idx       = iter.idx;
        arr_size  = iter.arr_size;
        item0     = iter.item0;
      }
      return *this;
    }

    template <typename DIR>
    cl_iterator_storage& operator=(cl_iterator_storage<STOR, DIR> const& iter)
    {
      idx       = iter.index();
      arr_size  = iter.array_size();
      item0     = iter.get_item0();
      return *this;
    }

  protected:
    operator bool() const
    {
      return valid();
    }

  public:
    bool valid() const
    {
      return idx < array_size();
    }

    bool valid(::size_t const& index) const
    {
      return index < array_size();
    }

    bool next()
    {
      DIRECTION::next(*this);
      return *this;
    }

    bool prev()
    {
      DIRECTION::prev(*this);
      return *this;
    }

    void reset()
    {
      DIRECTION::reset(*this);
    }

    void deface()
    {
      DIRECTION::deface(*this);
    }

    ::size_t array_size() const
    {
      return arr_size;
    }

    ::size_t index() const
    {
      return idx;
    }

    STOR get_item0() const
    {
      return item0;
    }

  };


  template <typename T, typename DIR1, typename DIR2>
  bool operator == 
  (
    cl_iterator_storage<T, DIR1> const& lhs
  , cl_iterator_storage<T, DIR2> const& rhs
  )
  {
    return lhs.get_item0() == rhs.get_item0() && lhs.index() == rhs.index();
  }


  template <typename T, typename DIR1, typename DIR2>
  bool operator != 
  (
    cl_iterator_storage<T, DIR1> const& lhs
  , cl_iterator_storage<T, DIR2> const& rhs
  )
  {
    return !(lhs == rhs);
  }


#ifdef CFG_RVA
#  define IMAGE_BASE_DEFAULT_VALUE
#else
#  define IMAGE_BASE_DEFAULT_VALUE  =0
#endif


  template <typename ITER_TR, typename DIRECTION>
  class cl_value_array_iterator
    : public cl_iterator_storage<typename ITER_TR::STOR, DIRECTION>
  {
    typedef cl_iterator_storage<typename ITER_TR::STOR, DIRECTION> BASE;
    typedef typename ITER_TR::DST   DST;
    typedef typename ITER_TR::STOR  STOR;
    typedef typename ITER_TR::RET   RET;

  public:
    template <typename SRC>
    explicit cl_value_array_iterator
    (
      SRC       const* const  src
    , ::size_t  const&        img_base IMAGE_BASE_DEFAULT_VALUE
    )
      : BASE
        (
          cl_info<DST, SRC, SRC>::cl_data_cast(src, img_base)
        , src->*cl_info<DST, SRC, SRC>::array_size_offset
        )
    {
    }                      

    template <typename A_SRC, typename SZ_SRC>
    cl_value_array_iterator
    (
      A_SRC     const* const  addr_src
    , SZ_SRC    const* const  sz_src
    , ::size_t  const&        img_base IMAGE_BASE_DEFAULT_VALUE
    )
      : BASE
        (
          cl_info<DST, A_SRC, SZ_SRC>::cl_data_cast(addr_src, img_base)
        , sz_src->*cl_info<DST, A_SRC, SZ_SRC>::array_size_offset
        )
    {
    }                      

    cl_value_array_iterator(cl_value_array_iterator const& iter)
      : BASE ( iter )
    {
    }


    template <typename DIR>
    cl_value_array_iterator(cl_value_array_iterator<ITER_TR, DIR> const& iter)
      : BASE ( iter )
    {
    }

  public:
    RET operator*() const
    {
      return &BASE::item0[idx];
    }

    RET operator->() const
    {
      return this->operator*();
    }

    cl_value_array_iterator& operator[](::size_t const& index)
    {
      DIRECTION::move(*this, index);
      return *this;
    }

  };


  template <typename ITER_TR, typename DIRECTION>
  class cl_ptr_array_iterator
    : public cl_iterator_storage<typename ITER_TR::STOR, DIRECTION>
  {
    typedef cl_iterator_storage<typename ITER_TR::STOR, DIRECTION> BASE;
    typedef typename ITER_TR::DST   DST;
    typedef typename ITER_TR::STOR  STOR;
    typedef typename ITER_TR::RET   RET;

    ::size_t image_base;
  
  public:
    template <typename SRC>
    explicit cl_ptr_array_iterator
    (
      SRC       const* const  src
    , ::size_t  const&        img_base IMAGE_BASE_DEFAULT_VALUE
    )
      : BASE
        (
          cl_info<DST, SRC, SRC>::cl_data_cast(src, img_base)
        , src->*cl_info<DST, SRC, SRC>::array_size_offset
        )
      , image_base ( img_base )
    {
    }                      

    template <typename A_SRC, typename SZ_SRC>
    cl_ptr_array_iterator
    (
      A_SRC     const* const  addr_src
    , SZ_SRC    const* const  sz_src
    , ::size_t  const&        img_base IMAGE_BASE_DEFAULT_VALUE
    )
      : BASE
        (
          cl_info<DST, A_SRC, SZ_SRC>::cl_data_cast(addr_src, img_base)
        , sz_src->*cl_info<DST, A_SRC, SZ_SRC>::array_size_offset
        )
      , image_base ( img_base )
    {
    }                      

    cl_ptr_array_iterator(cl_ptr_array_iterator const& iter)
      : BASE       ( iter )
      , image_base ( iter.image_base )
    {
    }

    template <typename DIR>
    cl_ptr_array_iterator(cl_ptr_array_iterator<ITER_TR, DIR> const& iter)
      : BASE       ( iter )
      , image_base ( iter.get_image_base() )
    {
    }

  public:
    RET operator*() const
    {
    #ifndef CFG_RVA
      return item0[idx];
    #else
      RET ptr = 0;
      if (rva_t rva = item0[idx])
      {
        ptr = reinterpret_cast<RET>(image_base + rva);
      }
      return ptr;
    #endif  //  CFG_RVA
    }

    RET operator->() const
    {
      return this->operator*();
    }

    cl_ptr_array_iterator& operator[](::size_t const& index)
    {
      DIRECTION::move(*this, index);
      return *this;
    }

    ::size_t const& get_image_base() const
    {
      return image_base;
    }
  };


  template <typename ITER_TR>
  class cl_single_value_iterator
  {
    typedef typename ITER_TR::DST   DST;
    typedef typename ITER_TR::STOR  STOR;
    typedef typename ITER_TR::RET   RET;

    STOR item;

    operator bool() const
    {
      return 0 != this->operator*();
    }        

  public:
    template <typename SRC>
    explicit cl_single_value_iterator
    (
      SRC       const* const  src
    , ::size_t  const&        img_base IMAGE_BASE_DEFAULT_VALUE
    )
      : item ( cl_info<DST, SRC, SRC>::cl_data_cast(src, img_base) )
    {}

    explicit cl_single_value_iterator(STOR const& src)
      : item ( src )
    {}

  public:
    bool valid() const
    {
      return 0 != this->operator*();
    }        

    RET operator*() const
    {
      return item;
    }

    RET operator->() const
    {
      return this->operator*();
    }

  };


#ifdef IMAGE_BASE_DEFAULT_VALUE
#undef IMAGE_BASE_DEFAULT_VALUE
#endif


}  //  namespace aux_
}  //  namespace msvc_internal_data_aux
}  //  namespace cpprtl


#endif  // include guard


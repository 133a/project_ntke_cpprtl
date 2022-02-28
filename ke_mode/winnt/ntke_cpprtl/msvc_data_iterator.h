//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef MSVC_DATA_ITERATOR_H_
#define MSVC_DATA_ITERATOR_H_


//============================
// msvc internal data iterator
//----------------------------
// CPPRTL_CFG_RVA, rva_t, imagebase_t, size_t should be declared in a library config header

namespace cpprtl { namespace msvc_data_aux
{
  template <typename DATA> struct descriptor2;

  template <typename T>
  T rva_cast(rva_t const rva, imagebase_t const image_base)
  {
    if ( rva )
    {
      return reinterpret_cast<T>(size_t(image_base + rva));
    }
    return 0;
  }


  template <typename T>
  struct direct
  {
    typedef T        value_type;
    typedef T const& ref_type;
    typedef T const* ptr_type;

  private:
    typedef T const* stor_type;
    stor_type ptr;

  public:
    explicit direct(stor_type const p, imagebase_t = 0) : ptr ( p ) {}
    // iterator
    ref_type operator[](size_t const idx) const
    {
      return ptr[idx];
    }
    // accessor
    ref_type operator*() const
    {
      return *ptr;
    }
    // accessor
    ptr_type operator->() const
    {
      return ptr;
    }
    // accessor
    operator void const*() const
    {
      return static_cast<void const*>(ptr);
    }
    bool operator==(direct const& that) const
    {
      return this->ptr == that.ptr;
    }
  };

  template <typename T>
  struct indirect
  {
    typedef T        value_type;
    typedef T const& ref_type;
    typedef T const* ptr_type;

#ifdef CPPRTL_CFG_RVA
  private:
    typedef rva_t const* stor_type;
    stor_type   rva;
    imagebase_t image_base;

  public:
    indirect(stor_type const rva, imagebase_t const image_base) : rva ( rva ) , image_base ( image_base ) {}
    ref_type operator[](size_t const idx) const
    {
      return *rva_cast<ptr_type>(rva[idx], image_base);
    }
    bool operator==(indirect const& that) const
    {
      return (this->rva == that.rva) && (this->image_base == that.image_base);
    }
#else
  private:
    typedef T const* const* stor_type;
    stor_type ppt;

  public:
    explicit indirect(stor_type const p, imagebase_t = 0) : ppt ( p ) {}
    ref_type operator[](size_t const idx) const
    {
      return *ppt[idx];
    }
    bool operator==(indirect const& that) const
    {
      return this->ppt == that.ppt;
    }
#endif  // CPPRTL_CFG_RVA
  };


  template <typename ACS>  // direct | indirect
  class iterator
  {
    typedef ACS access_type;

    size_t      idx;
    size_t      arr_size;
    access_type access;

  public:
    // indexation
    bool valid(size_t const i) const { return i < arr_size; }
    bool valid() const { return idx < arr_size; }
    size_t index() const { return idx; }
    size_t size()  const { return arr_size; }
    iterator& deface() { idx  = arr_size; return *this; }
    iterator& next()   { ++idx; return *this; }
    iterator& prev()   { --idx; return *this; }
    iterator& first()  { idx = 0; return *this; }
    iterator& last()   { idx = arr_size-1; return *this; }

    // access
    typename access_type::ref_type operator[](size_t const i) { return access[idx = i]; }
    typename access_type::ref_type operator*()  const { return  access[idx]; }
    typename access_type::ptr_type operator->() const { return &access[idx]; }

    // loading from descriptor
    template <typename DSC>
    iterator
    (
      DSC         const& dsc
    , imagebase_t const  image_base
    #ifndef CPPRTL_CFG_RVA
                                    = 0 
    #endif
    )
      : idx      ( 0 )
      , arr_size ( descriptor2<typename access_type::value_type>::size(dsc, image_base) )
      , access   ( descriptor2<typename access_type::value_type>::data(dsc, image_base), image_base )
    {}

    // comparison
    bool operator==(iterator const& that) const
    {
      return
      (
        ( this->access == that.access )  &&
        ( (this->idx == that.idx) || (!this->valid() && !that.valid()) )
      );
    }
    bool operator!=(iterator const& that) const
    {
      return !(*this == that);
    }
  };

#ifdef CPPRTL_CFG_RVA
  template <typename T>
  struct value
  {
    typedef T value_type;
    typedef T const& ref_type;
    typedef T const* ptr_type;

  private:
    value_type val;

  public:
    explicit value(ref_type v, imagebase_t = 0) : val ( v ) {}
    // accessor
    ref_type operator*() const { return val; }
  };
#endif  // CPPRTL_CFG_RVA

  template <typename ACS>  // direct | value
  class accessor
  {
    typedef ACS access_type;
    access_type access;

  public:
    template <typename DSC>
    accessor(DSC const& dsc, imagebase_t const image_base)
      : access ( descriptor2<typename access_type::value_type>::data(dsc, image_base), image_base )
    {}

    typename access_type::ref_type operator*()  const { return *access; }  // direct | value
    typename access_type::ptr_type operator->() const { return access.operator->(); }  // direct

    // == 0, direct
    operator void const*() const { return access; }
  };

}}  // namespace cpprtl::msvc_data_aux


#endif  // include guard

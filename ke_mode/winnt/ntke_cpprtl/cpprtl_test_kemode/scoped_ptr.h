/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef SCOPED_PTR_H_
#define SCOPED_PTR_H_


namespace aux_
{

  template <typename T>
  class scoped_ptr
  {
  public:
    typedef T  type;
    typedef T* pointer_type;
    typedef T& reference_type;

  private:
    pointer_type ptr;

  public:
    explicit scoped_ptr(pointer_type const& p) : ptr(p) {}

    ~scoped_ptr()
    {
      reset(0);
    }

    void reset(pointer_type const& p)
    {
      delete ptr;
      ptr = p;
    }

    pointer_type release()
    {
      pointer_type tmp = ptr;
      ptr = 0;
      return tmp;
    }

    pointer_type get() const
    {
      return ptr;
    }

    pointer_type operator->() const
    {
      return ptr;
    }

    reference_type operator*() const
    {
      return *ptr;
    }

  private:
    scoped_ptr(scoped_ptr const&);
    scoped_ptr& operator=(scoped_ptr const&);
  };


  template <typename T>
  class scoped_array_ptr
  {
  public:
    typedef T   type;
    typedef T*  pointer_type;
    typedef T&  reference_type;

  private:
    pointer_type ptr;

  public:
    explicit scoped_array_ptr(pointer_type const& p) : ptr(p) {}

    ~scoped_array_ptr()
    {
      reset(0);
    }

    void reset(pointer_type const& p)
    {
      delete[] ptr;
      ptr = p;
    }

    pointer_type release()
    {
      pointer_type tmp = ptr;
      ptr = 0;
      return tmp;
    }

    pointer_type get() const
    {
      return ptr;
    }

    reference_type operator[](unsigned const& idx) const
    {
      return ptr[idx];
    }

  private:
    scoped_array_ptr(scoped_array_ptr const&);
    scoped_array_ptr& operator=(scoped_array_ptr const&);
  };

}  //  namespace aux_


#endif // include guard


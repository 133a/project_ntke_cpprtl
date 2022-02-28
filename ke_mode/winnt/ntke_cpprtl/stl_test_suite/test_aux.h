//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef TEST_AUX_H_
#define TEST_AUX_H_


#include <exception>
#include <utility>
#ifdef NT_KERNEL_MODE
  #include "test_kalloc.h"
#else
  #include <memory>
#endif


namespace cpprtl { namespace stl { namespace test
{
  struct test_error
    : public std::exception
  {
    int err;

    test_error(int const e)
      : err ( e )
    {}
  };


  class context
  {
    int ctor_count;
    int cctor_count;
    int dtor_count;
    int xtor_count;
    int const expected;
    int state;

  public:
    explicit context(int const expected)
      : ctor_count  ( 0 )
      , cctor_count ( 0 )
      , dtor_count  ( 0 )
      , xtor_count  ( 0 )
      , expected    ( expected )
      , state       ( 0 )
    {}

    void ctor()
    {
      ++ctor_count;
      ++xtor_count;
    }

    void cctor()
    {
      ++cctor_count;
      ++xtor_count;
    }

    void dtor()
    {
      ++dtor_count;
      --xtor_count;
    }

    bool ok() const
    {
      return (state - expected == 0) &&
             (xtor_count + (ctor_count + cctor_count - dtor_count) == 0);
    }

  private:
    context(context const&);
    context& operator=(context&);
  };


  struct xtor_counter
  {
    context& ctx;

    explicit xtor_counter(context& c)
      : ctx ( c )
    {
      ctx.ctor();
    }

    xtor_counter(xtor_counter const& src)
      : ctx ( src.ctx )
    {
      ctx.cctor();
    }

    virtual ~xtor_counter()
    {
      ctx.dtor();
    }
  };


  template <typename T>
  class test_object
    : public xtor_counter
  {
    T val;

  public:
    typedef T value_type;

    explicit test_object(context& c, T const& t = T())
      : xtor_counter ( c )
      , val          ( t )
    {}

    test_object(test_object const& src)
      : xtor_counter ( src )
      , val          ( src.value() )
    {}

    test_object& operator=(test_object const& rhs)
    {
      if ( &rhs != this )
      {
        val = rhs.value();
      }
      return *this;
    }

  public:
    T value() const
    {
      return val;
    }

    void value(T const& t)
    {
      val = t;
    }
  };


  template <typename T1, typename T2> 
  bool operator==(test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return lhs.value() == rhs.value();
  }

  template <typename T1, typename T2> 
  bool operator!=(test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return ! ( lhs == rhs );
  }

  template <typename T1, typename T2> 
  bool operator<(test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return lhs.value() < rhs.value() ;
  }

  template <typename T1, typename T2> 
  bool operator>(test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return lhs.value() > rhs.value() ;
  }


  template <typename T>
  class test_object_init
  {
  protected:
    typename T::value_type cnt;

  public:
    test_object_init()
      : cnt ( 0 )
    {}

    void operator()(T& item)
    {
      item.value(cnt++);
    }
  };


  template <typename T>
  class test_object_sum
  {
  protected:
    typename T::value_type sum;

  public:
    typedef void     result_type;
    typedef T const& argument_type;
    typedef typename T::value_type value_type;

    test_object_sum()
      : sum ( 0 )
    {}

    void operator()(T const& item)
    {
      sum += item.value();
    }

    value_type get() const
    {
      return sum;
    }
  };


#ifdef NT_KERNEL_MODE
  typedef nonpaged_allocator <test_object<int> >  int_obj_nonpaged_allocator;
  typedef paged_allocator    <test_object<int> >  int_obj_paged_allocator;
  typedef nonpaged_allocator <std::pair<int, test_object<int> > >  int_obj_pair_nonpaged_allocator;
  typedef paged_allocator    <std::pair<int, test_object<int> > >  int_obj_pair_paged_allocator;
#else
  typedef std::allocator <test_object<int> >  int_obj_nonpaged_allocator;
  typedef std::allocator <test_object<int> >  int_obj_paged_allocator;
  typedef std::allocator <std::pair<int, test_object<int> > >  int_obj_pair_nonpaged_allocator;
  typedef std::allocator <std::pair<int, test_object<int> > >  int_obj_pair_paged_allocator;
#endif

}}}  // namespace cpprtl::stl::test


#endif  // include guard

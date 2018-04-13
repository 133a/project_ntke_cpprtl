/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TEST_CONTEXT_H_
#define TEST_CONTEXT_H_


namespace aux_
{
  enum
  {
    STATE_FACTOR  = 10000
  , XTOR_FACTOR   = 1
  };


  struct test_context
  {
    int const state_expected;
    int  state;
    int  flags;

    int  ctor_count;
    int  cctor_count;
    int  dtor_count;
    int  xtor_count;

    explicit test_context(int expected)
      : state_expected  ( expected )
      , state           ( 999 )
      , flags           ( 0 )
      , ctor_count      ( 0 )
      , cctor_count     ( 0 )
      , dtor_count      ( 0 )
      , xtor_count      ( 0 )
    {
    }

    int xtor_balance() const
    {
      return  xtor_count + ( ctor_count + cctor_count - dtor_count );
    }

    int state_balance() const
    {
      return state_expected - state;
    }

    int balance() const
    {
      return state_balance() * STATE_FACTOR + xtor_balance() * XTOR_FACTOR;
    }

  private:  // context is intended to be the single instance in a test
    test_context(test_context const&);
    test_context& operator=(test_context&);
  };


  class test_object_base
  {
    test_context& ctx;

  protected:
    explicit test_object_base(test_context& c_)
      : ctx  ( c_ )
    {
      ++ctx.ctor_count;
      ++ctx.xtor_count;
    }

    test_object_base(test_object_base const& src)
      : ctx  ( src.ctx )
    {
      ++ctx.cctor_count;
      ++ctx.xtor_count;
    }

    ~test_object_base()
    {
      ++ctx.dtor_count;
      --ctx.xtor_count;
    }

  };



  template <typename T>
  class test_object
    : public test_object_base
  {
    T val;

  public:
    typedef T value_type;

    explicit test_object(test_context& c, T const& t = T())
      : test_object_base ( c )
      , val              ( t )
    {}

    test_object(test_object const& src)
      : test_object_base ( src )
      , val              ( src.value() )
    {}

    test_object& operator = (test_object const& rhs)
    {
      if ( &rhs != this )
      {
        val = rhs.value();
      }
      return *this;
    }

    virtual ~test_object()
    {}

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
  bool operator == (test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return lhs.value() == rhs.value();
  }


  template <typename T1, typename T2> 
  bool operator != (test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return ! ( lhs == rhs );
  }


  template <typename T1, typename T2> 
  bool operator < (test_object<T1> const& lhs, test_object<T2> const& rhs)
  {
    return lhs.value() < rhs.value() ;
  }


  template <typename T1, typename T2> 
  bool operator > (test_object<T1> const& lhs, test_object<T2> const& rhs)
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

    void operator () (T& item)
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
    typedef void       result_type;
    typedef T const&   argument_type;
    typedef typename T::value_type value_type;

    test_object_sum()
      : sum ( 0 )
    {}

    void operator () (T const& item)
    {
      sum += item.value();
    }

    value_type get() const
    {
      return sum;
    }
  };


}  // namespace aux_


#endif  // include guard


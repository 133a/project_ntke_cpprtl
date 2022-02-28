//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// bad_cast/bad_typeid tests
// MT-safe
#include "test_aux.h"
#ifdef NT_KERNEL_MODE
  #include "ntddk.include.h"
#endif

namespace
{
  struct no_rtti_data
  {
    size_t data;
    no_rtti_data() : data ( 2*sizeof(size_t) ) {}
  };

  struct base0 { int a; int b; virtual ~base0() {} };
  class base1 : public base0 { int a; int b; };
  class base2 : public base1 { int a; };
  class derived : public base2 { int a; int b; };

#if 0
  base0
    |
  base1
    |
  base2
    |
  derived
#endif  // 0


  bool is_irql_passive_level()
  {
  #ifdef NT_KERNEL_MODE
    return KeGetCurrentIrql() == PASSIVE_LEVEL;
  #else
    return true;
  #endif
  }
}  // namespace

namespace cpprtl { namespace rtti { namespace test
{
//====================
// dynamic_cast<void*>
//--------------------
  bool test_b0101()
  try
  {
    if ( !is_irql_passive_level() ) { return true; }
    no_rtti_data n;
    base1* const pb1_rei = reinterpret_cast<base1*>(&n);
    void* const  pdyn    = dynamic_cast<void*>(pb1_rei);
    return (void)pdyn, false;
  }
  catch (std::bad_cast&)
  {
    return true;
  }                            
  catch (std::bad_typeid&)  // msvcrt throws `__non_rtti_object' derived from `bad_typeid'
  {
    return true;
  }
  catch (...)
  {
    return false;
  }

//===============
// dynamic_cast<>
//---------------
  bool test_b0102()
  try
  {
    if ( !is_irql_passive_level() ) { return true; }
    no_rtti_data n;
    base0&   rb0_rei = reinterpret_cast<base0&>(n);
    derived& rd_dyn  = dynamic_cast<derived&>(rb0_rei);
    return (void)rd_dyn, false;
  }
  catch (std::bad_cast&)
  {
    return true;
  }                            
  catch (std::bad_typeid&)  // msvcrt throws `__non_rtti_object' derived from `bad_typeid'
  {
    return true;
  }
  catch (...)
  {
    return false;
  }

//=======
// typeid
//-------
  bool test_b0103()
  try
  {
    if ( !is_irql_passive_level() ) { return true; }
    no_rtti_data n;
    base2& rb2_rei = reinterpret_cast<base2&>(n);
    std::type_info const& ti = typeid(rb2_rei);
    return (void)ti, false;
  }
  catch (std::bad_typeid&)
  {
    return true;
  }
  catch (...)
  {
    return false;
  }

}}}  // namespace cpprtl::rtti::test

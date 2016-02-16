/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  basic rtti tests - public single inheritance, cast-to-void, typeid
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>


#ifdef NT_KERNEL_MODE
  extern "C" unsigned char __stdcall KeGetCurrentIrql();
  #define PASSIVE_LEVEL   0
  #define APC_LEVEL       1
#endif  //  NT_KERNEL_MODE


namespace
{
  class no_virtual
  {
    static void const* const p;

    void const* const p00;  void const* const p01;  void const* const p02;  void const* const p03;
    void const* const p04;  void const* const p05;  void const* const p06;  void const* const p07;

  public:
    no_virtual()
      : p00(p) , p01(p) , p02(p) , p03(p)
      , p04(p) , p05(p) , p06(p) , p07(p)
    {}
  };
  void const* const no_virtual::p = reinterpret_cast<void*>(sizeof(void*));


  class base00
  {
    int a;
    int b;
    int c;
  public:
    virtual ~base00() {}
  };


  class base0
    : protected base00
  {
    int a;
    int b;
    int c;
  public:
    virtual ~base0() {}
  };


  class base1
    : public base0
  {
    int a;
    int b;
  };


  class base2
    : public base1
  {
    int a;
  };


  class derived
    : public base2
  {
    int d;
    int e;
  };

/**************************************************

     base00
       +
       +
     base0
       |
       |
     base1
       |
       |
     base2
       |
       |
    derived

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{

  int test_si01()
  {
    int ret = 0;

    try
    {

      ret = 0;
      no_virtual nv;
      base1 b;
      derived d;


      // compile-time resolved dynamic_cast
      ++ret;
      {
        base1* const pb_sta = &d;
        base1* const pb_dyn = dynamic_cast<base1*>(&d);
        if (pb_sta != pb_dyn)
        {
          throw ret;
        }
      }


      // dynamic up-cast to the not-visible base
      // icl error #312: conversion to inaccessible base class "<unnamed>::base00" is not allowed
      // bcc64 error: cannot cast '<anonymous>::derived' to its protected base class '<anonymous>::base00'
      // msvc warning C4540: dynamic_cast used to convert to inaccessible or ambiguous base; run-time test will fail
      ++ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base00* const pb00_dyn = dynamic_cast<base00*>(&d);
        if ( pb00_dyn != 0)
        {
          throw ret;
        }
      }
#endif


      // dynamic down-cast from the not-visible base
      ++ret;
      {
        base00* const pb00_sta = (base00*)&d;
        derived* const pd_dyn = dynamic_cast<derived*>(pb00_sta);
        if ( pd_dyn != 0)
        {
          throw ret;
        }
      }


      // run-time resolved dynamic_cast
      ++ret;
      {
        base1* const pb_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(pb_sta);
        if (pd_dyn != &d)
        {
          throw ret;
        }
      }


      // run-time failed dynamic_cast
      ++ret;
      {
        base1* const pb_sta = &b;
        derived* const pd_dyn = dynamic_cast<derived*>(pb_sta);
        if (pd_dyn != 0)
        {
          throw ret;
        }
      }


      // type_info check
      ++ret;
      {
        base1* const pb_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(pb_sta);
        if ( typeid(*pd_dyn) != typeid(*pb_sta) )
        {
          throw ret;
        }
      }


      // bad_cast check
      ++ret;
      {
        while (1)
        {
          try
          {
            base1& refb_sta = b;
            derived& refd_dyn = dynamic_cast<derived&>(refb_sta);
          }
          catch (std::bad_cast&)
          {
            break; // test completed;
          }
          catch (...)
          {
          }
          throw ret;
        }
      }


      // cast to void*
      ++ret;
      {
        base1* const pb1_sta = &b;
        base1* const pb2_sta = &d;
        derived* const pd2_sta = &d;

        void* const pb1_dyn = dynamic_cast<void*>(pb1_sta);
        if (pb1_dyn != pb1_sta)
        {
          throw ret;
        }

        void* const pd2_dyn = dynamic_cast<void*>(pb2_sta);
        if (pd2_dyn != pd2_sta)
        {
          throw ret;
        }
      }


//////////////////////////////////////////////////////
////
////  the following tests are based on the assumption that rtti-lib will dereference the null-ptrs ('no_virtual' type's objects are filled with ptr=(0+sizeof(void*)) and get access violation at invalid u-mode address=0.
////
////  in ke-mode the following checks with an arbitrary-filled 'no_virtual'-type may lead to the BSOD in get_complete_object_locator() -
////  - because of it's probably unable to check whether the kernel space address is valid (getting the bound trap if not).
////
////  more over - if the ke- or um-address of the fake COL accidentally gets a valid value which is (too accidentally) pointing to a valid memory area -
////  - we end up with a wrong resulting cast-to-void or typeid or less probably dynamic_cast (because of more data structures involved)
////
////  so don't use such a tricks as following - dynamically cast only trusted polymorphic objects' pointers - not some kind reinterpreted ones !
////
//////////////////////////////////////////////////////

#ifdef NT_KERNEL_MODE
      enum { KE_SKIP_TESTS = 3 };
      if ( KeGetCurrentIrql() > PASSIVE_LEVEL )  //  skip from this point
      {
        ret += KE_SKIP_TESTS;                        //  adjusting the test counter,
        goto IF_IRQL_IS_GREATER_THAN_PASSIVE_LEVEL;  //  if current IRQL>PASSIVE_LEVEL an access violation at a user space address becomes the bound trap, so let's skip.
      }
#endif  //  NT_KERNEL_MODE


      // badly designed to get bad_cast by access violation while casting to void*
      ++ret;
      {
        while (1)
        {
          try
          {
            no_virtual* const pnv_sta = &nv;
            base1* const pb_rei = reinterpret_cast<base1*>(pnv_sta);
            void* const pb_dyn = dynamic_cast<void*>(pb_rei);
#ifndef _MSC_VER
            break;   // others (e.g. bcc64's cxxabi-lib) sometimes get here, but we don't care
#endif
          }
          catch (std::bad_cast&)
          {
            break; // test completed;
          }                            
          catch (std::bad_typeid&)  // msvcrt throws '__non_rtti_object' type derived from 'bad_typeid'
          {
            break; // test completed;
          }
          catch (...)
          {
          }
          throw ret;
        }
      }


      // badly designed to get bad_cast by access violation
      ++ret;
#ifndef __CODEGEARC__  //  bcc64's cxxabi-lib makes access violation
      {
        while (1)
        {
          try
          {
            no_virtual& refnv_sta = nv;
            base1& refb_rei = reinterpret_cast<base1&>(refnv_sta);
            derived& refd_dyn = dynamic_cast<derived&>(refb_rei);
          }
          catch (std::bad_cast&)
          {
            break; // test completed;
          }
          catch (std::bad_typeid&)  // msvcrt throws '__non_rtti_object' type derived from 'bad_typeid'
          {
            break; // test completed;
          }
          catch (...)
          {
          }
          throw ret;
        }
      }
#endif


      // badly designed to get bad_typeid by access violation
      ++ret;
#ifndef __CODEGEARC__  //  bcc64 cxxabi-lib's typeid() returns hanging ref here
      {
        while (1)
        {
          try
          {
            no_virtual& refnv_sta = nv;
            base1& refb_rei = reinterpret_cast<base1&>(refnv_sta);
            std::type_info const& ti = typeid(refb_rei);
            char const* tname = ti.name();
          }
          catch (std::bad_typeid&)
          {
            break; // test completed;
          }
          catch (...)
          {
          }
          throw ret;
        }

      }
#endif


#ifdef NT_KERNEL_MODE
IF_IRQL_IS_GREATER_THAN_PASSIVE_LEVEL: ;  //  'IRQL>PASSIVE_LEVEL'  skip to this point
#endif

      ret = 0; // one would go here only if tests are successfully passed
    }
    catch (int& i)
    {
      ret = i;
    }
    catch (std::bad_cast&)
    {
      ret = -ret;
    }
    catch (std::bad_typeid&)
    {
      ret = -ret;
    }
    catch (...)
    {
      ret = -ret;
    }
    return ret;
  }


}  }  }


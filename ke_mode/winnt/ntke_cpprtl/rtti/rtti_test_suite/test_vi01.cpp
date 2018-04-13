/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  virtual public inheritance tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>
#include "rtti_test_defs.h"


namespace
{
  int const A_VAL = 987654;


  class base00 { public : int a; virtual ~base00() {} base00() : a(A_VAL) {} };

  class base10 : virtual public base00 { int a; virtual void b10() {} };  // virtual function is put to force the icl to generate rtti info
  class base11 : virtual public base00 { int a; virtual void b11() {} };  // virtual function is put to force the icl to generate rtti info

  class base20 : public base10 { int a; };
  class base21 : public base11 { int a; };

  class derived : public base20, public base21 { int a; };

/**************************************************

              base00
                /\
               /  \
              /    \
          base10   base11
             |      |
          base20   base21
              \    /
               \  /
                \/
             derived
        

***************************************************/

}


namespace cpprtl { namespace test { namespace rtti
{


  int test_vi01()
  {
    int ret = RTTI_OK;
    derived d;

    try
    {

      // cast-to-void*
      ++ret;
      {
        base10* const b10_sta = &d;
        base11* const b11_sta = &d;
        base20* const b20_sta = &d;
        base21* const b21_sta = &d;
        derived* const d_sta  = &d;
        void* const p10_dyn = dynamic_cast<void*>(b10_sta);
        void* const p11_dyn = dynamic_cast<void*>(b11_sta);
        void* const p20_dyn = dynamic_cast<void*>(b20_sta);
        void* const p21_dyn = dynamic_cast<void*>(b21_sta);
        void* const pd_dyn  = dynamic_cast<void*>(d_sta);

        if
        (
           ! p10_dyn
          || p10_dyn != p11_dyn
          || p11_dyn != p20_dyn
          || p20_dyn != p21_dyn
          || p21_dyn != pd_dyn
        )
        {
          throw ret;
        }
      }

    
      // down-cast
      ++ret;
      {
        base11* const b11_sta = &d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b11_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }

    
      // down-cast
      ++ret;
      {
        base10* const b10_sta = &d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b10_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }

    
      // left-right cross-cast
      ++ret;
      {
        base20* const b20_sta = &d;
        base11* const b11_sta = &d;
        base11* const b11_dyn = dynamic_cast<base11*>(b20_sta);

        if ( b11_dyn != b11_sta )
        {
          throw ret;
        }
      }

    
      // right-left cross-cast
      ++ret;
      {
        base11* const b11_sta = &d;
        base10* const b10_sta = &d;
        base10* const b10_dyn = dynamic_cast<base10*>(b11_sta);

        if ( b10_dyn != b10_sta )
        {
          throw ret;
        }
      }


      // cast to virtual base
      ++ret;
      {
        base00* const b00_dyn = dynamic_cast<base00*>(&d);

        if ( b00_dyn == 0 )
        {
          throw ret;
        }

        try
        {
          if ( A_VAL != b00_dyn->a)
          {
            throw ret;
          }
        }
        catch (int&)
        {
          throw;
        }
        catch (...)
        {
          throw ret;
        }
      }


      // cast to void* from virtual base
      ++ret;
      {
        derived* d_sta = &d;
        base00* const b00_dyn = dynamic_cast<base00*>(d_sta);
        void* const p = dynamic_cast<void*>(b00_dyn);
        if ( p != d_sta )
        {
          throw ret;
        }
      }


      ret = RTTI_OK;  // one would get here only if tests passed successfully
    }
    catch (int& i)
    {
      ret = i;
    }
    catch (std::bad_cast&)
    {
      ret += ERROR_BAD_CAST;
    }
    catch (std::bad_typeid&)
    {
      ret += ERROR_BAD_TYPEID;
    }
    catch (...)
    {
      ret += ERROR_UNEXPECTED;
    }
    return ret;
  }


}  }  }


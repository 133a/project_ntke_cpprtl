/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////
////
////  virtual inheritance with ambiguity tests
////  MT-safe
/////////////////////////////////////////////////////


#include <typeinfo>


namespace
{
  int const A_VAL = 7654;


  class base000 { public : int a; virtual ~base000() {} base000() : a(A_VAL) {} };

  class base100 : virtual public base000 { int a; virtual void b100() {} };  // virtual function is put to force the icl to generate rtti info

  class base20 : public base100 { int a; };
  class base21 : public base100 { int a; };

  class derived : public base20, public base21 { int a; };

/**************************************************

              base000
                /\
               /  \
              /    \
         base100   base100
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

  int test_vi03()
  {
    int ret = 0;
    derived d;

    try
    {

  ////// cast to virtual base
      ++ ret;
      {
        base000* const b000_dyn = dynamic_cast<base000*>(&d);

        if ( b000_dyn == 0 )
        {
          throw ret;
        }

        try
        {
          if ( A_VAL != b000_dyn->a)
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


  ////// cast to void* from virtual base
      ++ ret;
      {
        derived* d_sta = &d;
        base000* const b000_dyn = dynamic_cast<base000*>(d_sta);
        void* const p = dynamic_cast<void*>(b000_dyn);
        if ( p != d_sta )
        {
          throw ret;
        }
      }


  ////// cast-to-void*
      ++ ret;
      {
        base20* const b20_sta     = &d;
        base21* const b21_sta     = &d;
        base100* const b100_sta0  = b20_sta;
        base100* const b100_sta1  = b21_sta;
        derived* const d_sta      = &d;
        void* const p100_dyn0   = dynamic_cast<void*>(b100_sta0);
        void* const p100_dyn1   = dynamic_cast<void*>(b100_sta1);
        void* const p20_dyn     = dynamic_cast<void*>(b20_sta);
        void* const p21_dyn     = dynamic_cast<void*>(b21_sta);
        void* const pd_dyn      = dynamic_cast<void*>(d_sta);

        if
        (
           ! p100_dyn0
          || p100_dyn0  !=  p100_dyn1
          || p100_dyn1  !=  p20_dyn
          || p20_dyn    !=  p21_dyn
          || p21_dyn    !=  pd_dyn
        )
        {
          throw ret;
        }
      }

    
  ////// down-casts
      // down-cast
      ++ ret;
      {
        base100* const b100_sta = (base20*)&d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b100_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }

    
      // down-cast
      ++ ret;
      {
        base100* const b100_sta = (base21*)&d;
        derived* const pd_sta = &d;
        derived* const pd_dyn = dynamic_cast<derived*>(b100_sta);

        if ( pd_dyn != pd_sta )
        {
          throw ret;
        }
      }


      // wrong down-cast
      ++ret;
      {
        base000* const b000_dyn = dynamic_cast<base000*>(&d);
        if ( b000_dyn == 0 )
        {
          throw ret;
        }

        base100* const b100_dyn = dynamic_cast<base100*>(b000_dyn);
        if ( b100_dyn != 0 )
        {
          throw ret;
        }
      }


  ////// up-casts
      // wrong up-cast
      // icl error: base class "<unnamed>::base100" is ambiguous
      // bcc64 error: ambiguous conversion from derived class '<anonymous>::derived' to base class '<anonymous>::base100'
      ++ret;
#if !defined (__ICL) && !defined (__CODEGEARC__)
      {
        base100* const b100_dyn = dynamic_cast<base100*>(&d);
        if ( b100_dyn != 0 )
        {
          throw ret;
        }
      }
#endif


  ////// left-right cross-casts
      // left-right cross-cast
      ++ ret;
      {
        base100* const b100_sta = (base20*)&d;
        base21* const b21_sta = &d;
        base21* const b21_dyn = dynamic_cast<base21*>(b100_sta);

        if ( b21_dyn != b21_sta )
        {
          throw ret;
        }
      }

    
  ////// right-left cross-casts
      // right-left cross-cast
      ++ ret;
      {
        base100* const b100_sta = (base21*)&d;
        base20* const b20_sta = &d;
        base20* const b20_dyn = dynamic_cast<base20*>(b100_sta);

        if ( b20_dyn != b20_sta )
        {
          throw ret;
        }
      }

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


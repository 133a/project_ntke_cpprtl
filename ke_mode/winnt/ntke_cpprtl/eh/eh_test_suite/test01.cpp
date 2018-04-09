/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing 'catch(...)'
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK = 0,
  };
}


namespace cpprtl { namespace test { namespace eh
{

  int test01()
  {
    context ctx(EH_OK);  // EH_OK is expected
  
    try
    {
      throw 1;
    }
    catch (...)
    {
      ctx.state = EH_OK;
    }
    return ctx.balance();
  }

}  }  }


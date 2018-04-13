/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing 'catch(UDT&)'
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK         = 0
  , MAGIC_VALUE3  = 33
  };
}


namespace cpprtl { namespace test { namespace eh
{

  int test03() 
  {
    context ctx(MAGIC_VALUE3);

    try
    {
      throw eh_test(ctx, MAGIC_VALUE3);
    }
    catch (eh_test& ex)
    {
      ctx.state = ex.i;
    }
 
    return ctx.balance();
  }

}  }  }


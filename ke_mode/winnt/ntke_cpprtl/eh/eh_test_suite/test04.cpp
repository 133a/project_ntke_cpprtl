/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing 'catch(UDT)'
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK         = 0
  , MAGIC_VALUE4  = 44
  };
}


namespace cpprtl { namespace test { namespace eh
{

  int test04()
  {
    context ctx(MAGIC_VALUE4);
 
    try
    {
      throw eh_test(ctx, MAGIC_VALUE4);
    }
    catch (eh_test ex)
    {
      ctx.state = ex.i;
    }
 
    return ctx.balance();
  }

}  }  }


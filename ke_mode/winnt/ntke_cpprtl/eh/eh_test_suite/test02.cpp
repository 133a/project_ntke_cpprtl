/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing catching of built-in types and subsequent try-blocks in one function
////  MT-safe
/////////////////////////////////////////////


#include "context.h"


namespace
{
  enum
  {
    EH_OK = 0,
  };


  int      const  int02  = 356;
  char     const  char02 = 'a';
  unsigned const  uint02 = 897;
}


namespace cpprtl { namespace test { namespace eh
{

  int test02()
  {
    context ctx(int02 + char02 + uint02);  // 3 catches are expected
    ctx.state = EH_OK;
  
    try
    {
      throw int02;
    }
    catch (int i)
    {
      ctx.state += i;
    }

    try
    {
      throw char02;
    }
    catch (char c)
    {
      ctx.state += c;
    }

    try
    {
      throw uint02;
    }
    catch (unsigned u)
    {
      ctx.state += u;
    }

    return ctx.balance();
  }

}  }  }


//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum { THROW = 11 };
    typedef xtor_counter<0> UDT;
    typedef xtor_counter<THROW> EXC;
  }

//=============
// 'catch(...)'
//-------------
  bool test_0101()
  {
    context ctx(THROW);
    try
    {
      throw 0;
    }
    catch (...)
    {
      ctx.state = THROW;
    }
    return ctx.ok();
  }

//==============
// 'catch(BIT&)'
//--------------
  bool test_0102()
  {
    typedef unsigned BIT;
    context ctx(THROW);
    try
    {
      throw BIT(THROW);
    }
    catch (BIT& t)
    {
      ctx.state = t;
    }
    return ctx.ok();
  }

//====================
// 'catch(BIT const&)'
//--------------------
  bool test_0103()
  {
    typedef long BIT;
    context ctx(THROW);
    try
    {
      throw BIT(THROW);
    }
    catch (BIT const& t)
    {
      ctx.state = t;
    }
    return ctx.ok();
  }

//=============
// 'catch(BIT)'
//-------------
  bool test_0104()
  {
    typedef int BIT;
    context ctx(THROW);
    try
    {
      throw BIT(THROW);
    }
    catch (BIT t)
    {
      ctx.state = t;
    }
    return ctx.ok();
  }

//==============
// 'catch(UDT&)'
//--------------
  bool test_0105() 
  {
    context ctx(THROW);
    try
    {
      throw EXC(ctx);
    }
    catch (EXC& exc)
    {
      ctx.state = exc.val;
    }
    return ctx.ok();
  }

//====================
// 'catch(UDT const&)'
//--------------------
  bool test_0106() 
  {
    context ctx(THROW);
    try
    {
      throw EXC(ctx);
    }
    catch (EXC const& exc)
    {
      ctx.state = exc.val;
    }
    return ctx.ok();
  }

//=============
// 'catch(UDT)'
//-------------
  bool test_0107() 
  {
    context ctx(THROW);
    try
    {
      throw EXC(ctx);
    }
    catch (EXC exc)
    {
      ctx.state = exc.val;
    }
    return ctx.ok();
  }

//===============
// 'catch(void*)'
//---------------
  bool test_0108() 
  {
    context ctx(THROW);
    int val = THROW;
    try
    {
      throw &val;
    }
    catch (void* ptr)
    {
      ctx.state = *reinterpret_cast<int*>(ptr);
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

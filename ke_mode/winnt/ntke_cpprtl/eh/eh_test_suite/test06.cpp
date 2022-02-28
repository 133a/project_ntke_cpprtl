//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// stack exhaustion tests
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {                    
    #ifdef NT_KERNEL_MODE
      #if   defined (_M_IX86)
        NESTED = 30  // stack overrun at 35
      #elif defined (_M_X64) || defined (_M_AMD64)
        NESTED = 18  // stack overrun at 24
      #elif defined (_M_ARM)
        NESTED = 12  // stack overrun at 16
      #elif defined (_M_ARM64)
        NESTED = 10  // stack overrun at TODO
      #else
      # error check $(target.arch)
      #endif                                             
    #else
      NESTED = 32
    #endif  // NT_KERNEL_MODE
    };

    enum
    {
      THROW = 3
    };

    typedef xtor_counter<0> UDT;
    typedef xtor_counter<THROW> EXC;

    void nested_throw(context& ctx, int depth)
    {
      UDT udt(ctx);
      try
      {
        UDT udt(ctx);
        if ( depth > 0 )
        {
          UDT udt(ctx);
          throw EXC(ctx);
        }
      }
      catch (EXC const& exc)
      {
        UDT udt(ctx);
        ctx.state += exc.val;
        nested_throw(ctx, --depth);
      }
    }
  }  // namespace

//======================================
// stack exhaustion by nested exceptions
//--------------------------------------
  bool test_0601()
  {
    context ctx(NESTED*THROW);
    nested_throw(ctx, NESTED);
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing SEH-catching by '...'
////  MT-safe
/////////////////////////////////////////////


#include <excpt.h>
#include "context.h"


#ifdef NT_KERNEL_MODE
  extern "C" unsigned char __stdcall KeGetCurrentIrql();
  #define PASSIVE_LEVEL  0
  #define APC_LEVEL      1
#endif  //  NT_KERNEL_MODE


namespace
{
  int volatile zero = 0;      // don't set const 'cos compiler will detect zero division -)

  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    UNEXPECTED_CATCH1         = -2,
    UNEXPECTED_CATCH2         = -3,
    CATCH1                    = 101,
    CATCH2                    = 102,
    CATCH_SEH                 = CATCH2,
    MAGIC_VALUE16             = 123,
  };
}  


namespace
{

  void ff_seh_access_violation()
  {
//  access violation at user mode address is able to be catched by both user and kernel mode SEH
    int volatile* volatile const p = 0;
    *p = MAGIC_VALUE16;
  }


  void ff_seh_zero_division()
  {
// a zero division is a bound trap in nt kernel (i.e. can't be catched by seh) so let's just replace this by yet another access violation
#ifndef NT_KERNEL_MODE
    int volatile i = MAGIC_VALUE16 / zero;
#else
    ff_seh_access_violation();
#endif
  }


  void ff_catch(context& ctx)
  {
    try
    {
      ff_seh_access_violation();
    }
    catch (float)
    {
      ctx.state = UNEXPECTED_CATCH1;
    }
    catch (...)
    {
      ctx.state = CATCH1;
    }

    if (CATCH1 == ctx.state)
    {
      try
      {
        ff_seh_zero_division();
      }
      catch (int)
      {
        ctx.state = UNEXPECTED_CATCH2;
      }
      catch (...)
      {
        ctx.state = CATCH2;  // end up here with -EHa excepting (NT_KERNEL_MODE && _M_ARM)
      }
    }
  }

} // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test16()
  {
#ifdef NT_KERNEL_MODE
    if ( KeGetCurrentIrql() > PASSIVE_LEVEL )
    {
    //  in kernel the access violation at a user space address is only allowed when IRQL==PASSIVE_LEVEL otherwise a bound trap is expected
      return 0;
    }
#endif  //  NT_KERNEL_MODE


// cpp-exceptions are built upon the seh so we can do the trick following
#if defined (_MSC_VER)
    context ctx(CATCH2);
    __try
    {
      ff_catch(ctx);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      ctx.state = CATCH_SEH;  // end up here with -EHs or (-EHa && NT_KERNEL_MODE && _M_ARM)
    }
    return ctx.balance();
#endif

    return 0;
  }

}  }  }


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
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
#endif  // NT_KERNEL_MODE


namespace
{
  int volatile zero = 0;  // don't set const 'cos compilers will detect zero division

  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    UNEXPECTED_CATCH1         = -2,
    CATCH1                    = 101,
    CATCH_SEH                 = CATCH1,
    MAGIC_VALUE16             = 123,
  };
}  


namespace
{

  void ff_seh_access_violation()
  {
  // access violation at user mode address is able to be catched by both user and kernel mode SEH
    int volatile* volatile const p = 0;
    *p = MAGIC_VALUE16;
  }


  void ff_seh_zero_division()
  {
  // zero division can't be catched by seh in ke-mode so let's just replace this by yet another access violation
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
      ctx.state = CATCH1;  // end up here with -EHa (if !(NT_KERNEL_MODE && _M_ARM))
    }
  }

}  // namespace


namespace cpprtl { namespace test { namespace eh
{

  int test16()
  {
#ifdef NT_KERNEL_MODE
    if ( KeGetCurrentIrql() > PASSIVE_LEVEL )
    {
    //  in ke-mode the access violation at user space addresses is only allowed for IRQL==PASSIVE_LEVEL
      return 0;
    }
#endif  // NT_KERNEL_MODE


#ifdef _MSC_VER
    context ctx(CATCH1);
    __try
    {
      ff_catch(ctx);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      ctx.state = CATCH_SEH;  // end up here with -EHs or (-EHa && NT_KERNEL_MODE && _M_ARM)
    }
    return ctx.balance();
#endif  // _MSC_VER

    return 0;
  }

}  }  }


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing the throwing of an unexisted exception
////  MT-safe
/////////////////////////////////////////////


#include <excpt.h>

#include "../eh_exception_code.h"
#include "context.h"


namespace
{

  enum
  {
    EH_OK                     = 0,
    EH_CONTEXT_DEFAULT        = -1,
    THROW_WORKS_IMPROPERLY    = -2,
    UNEXISTED_IS_CATCHED      = -3,
    UNEXPECTED_EXCEPTION_CODE = -4,
  };


  int just_throw()
  {
    try
    {
      throw;
      return THROW_WORKS_IMPROPERLY;
    }
    catch (...)
    {
      return UNEXISTED_IS_CATCHED;  // can't be catched because of the non-existing exception object
    }
    return UNEXISTED_IS_CATCHED; 
  }

}


namespace cpprtl { namespace test { namespace eh
{

  int test00()
  {

#ifdef _MSC_VER   // cpp-exceptions are built upon the seh so we can do the trick following
    context ctx(EH_OK);
    __try
    {
      ctx.state = just_throw();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
      if 
      (
           cpprtl::eh::EXCEPTION_CODE_CPP      == GetExceptionCode()
        || cpprtl::eh::EXCEPTION_CODE_MSFTCPP  == GetExceptionCode() 
      )
      {
        ctx.state = EH_OK;  // cpp exception would be propagated here
      }
      else
      {
        ctx.state = UNEXPECTED_EXCEPTION_CODE;
      }
    }
    return ctx.balance();
#endif  //  _MSC_VER

    return 0;
  }

}  }  }


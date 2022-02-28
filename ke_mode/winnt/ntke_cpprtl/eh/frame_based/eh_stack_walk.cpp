//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "eh_config.h"
#include "eh_framework.h"

#include "eh_seh_reg.h"
#include "eh_exception_code.h"
#include "eh_aux.h"


namespace cpprtl { namespace eh
{

  void stack_walk
  (
    ::EXCEPTION_RECORD            &       exc_rec
  , msvc_data::seh::registration  * const target_reg = 0  // when ==0 the dispathing is running, unwinding otherwise
  )
  {
    for
    (
      msvc_data::seh::registration* seh_reg = msvc_data::seh::head()
    ; msvc_data::seh::tail() != seh_reg
    ; seh_reg = target_reg ? msvc_data::seh::unlink(seh_reg) : seh_reg->next
    )
    {
      // check the frame registration plausibility
      seh_reg->verify();

      // target frame and unwind duties
      if ( target_reg )
      {
        if ( seh_reg > target_reg )  // the target frame looks unreachable
        {
          aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_UNREACHABLE_TARGET_FRAME);
        }
        if ( seh_reg == target_reg )  // the target frame has been reached
        {
          exc_rec.ExceptionFlags |= EXCEPTION_TARGET_UNWIND;
        }
        exc_rec.ExceptionFlags |= EXCEPTION_UNWINDING;
      }

      // run the frame handler and dispatch the frame disposition, CONTEXT and DISPATCHER_CONTEXT are not maintained
      switch ( seh_reg->frame_handler(&exc_rec, seh_reg, 0, 0) )
      {
      case ExceptionContinueSearch:  // proceed the next frame registration
        continue;
        break;

      case ExceptionDispatchCPP:  // delegate the exception handling duties back to the stack_walk() caller
        // do not unlink the target frame registration
        exc_rec.ExceptionFlags &= ~EXCEPTION_UNWIND;  // remove unwinding flags if any
        return;  
        break;

      default:
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_UNSUPPORTED_DISPOSITION);
        break;
      };

    }  // for(seh_reg)

    aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_STACKWALK_UNEXPECTED_RETURN);
  }

}}  // namespace cpprtl::eh

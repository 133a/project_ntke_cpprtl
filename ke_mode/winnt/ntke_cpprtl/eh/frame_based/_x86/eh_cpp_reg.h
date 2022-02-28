//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_CPP_REG_H_
#define EH_CPP_REG_H_


#include "eh_config.h"
#include "eh_seh_reg.h"
#include "eh_data.h"


namespace cpprtl { namespace eh { namespace msvc_data
{

  class cpp_registration  // SEH frame data for C++ functions
    : public seh::registration
  {
    frame_state_t state;  // C++ function frame state
    size_t        base;   // C++ function frame base (base=x86.ebp of the previous frame, &base=x86.ebp of the current frame)
  //size_t        return_address;

    using seh::registration::next;           // hide the base class data
    using seh::registration::frame_handler;  // ^ cast cpp_registration to seh::registration explicitly to access these names from outside

    // neither need to inherit the class nor to create objects, since
    // the compiler constructs this type objects as a part of C++ function stack frames;
    // __CxxFrameHandler() receives seh::registration* and downcasts to cpp_registration*
    cpp_registration();

  public:
    // frame pointers
    size_t frame_base() const
    {
      return reinterpret_cast<size_t>(&base);
    }
    size_t& stack_pointer() const  // the DWORD at the address '(size_t)this-4' of the C++ function frame is known to be the saved x86.esp of this frame
    {
      return const_cast<size_t&>((-1)[reinterpret_cast<size_t const*>(this)]);
    }

    // frame state access
    void frame_state(frame_state_t const s)
    {
      state = s;
    }
    frame_state_t frame_state(function_descriptor const& func_dsc) const
    {
      // Do not return just the value 'this->state', since overwriting state while unwinding may lead in
      // to a junk in the hi bytes of 'state' variable, due to msvc_x86 may set this variable by
      // the operation such as 'mov byte ptr [ebp-4],5' (writes only the lower byte of int) for functions with function_descriptor::unwind_array_size<=0x80. 
      // TODO check 'state' may be spoiled when a catch funclet or continuation is running after the frame has been unwound.
      return func_dsc.unwind_array_size > 0x80 ? state : static_cast<signed char>(state);
    }
  };

}}}  // namespace cpprtl::eh::msvc_data


#endif  // include guard

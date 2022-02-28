//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_EXCEPTION_CODE_H_
#define EH_EXCEPTION_CODE_H_


namespace cpprtl { namespace eh
{
  // exception codes
  enum
  {
    EXCEPTION_CODE_MSVC     = 0xE0000000 | 'msc'
  , EXCEPTION_CODE_CPP      = 0xE0000000 | 'c++'
  };

  // exception subcodes
  enum
  {
    EXCEPTION_SUBCODE_DEFAULT
  , EXCEPTION_SUBCODE_STACKWALK_BAD_STACK
  , EXCEPTION_SUBCODE_STACKWALK_FUNCTION_NOT_FOUND
  , EXCEPTION_SUBCODE_STACKWALK_UNREACHABLE_TARGET_FRAME
  , EXCEPTION_SUBCODE_STACKWALK_UNSUPPORTED_DISPOSITION
  , EXCEPTION_SUBCODE_STACKWALK_UNEXPECTED_RETURN
  , EXCEPTION_SUBCODE_BAD_REGISTRATION
  , EXCEPTION_SUBCODE_BAD_REGISTRATION_STACK
  , EXCEPTION_SUBCODE_BAD_REGISTRATION_SAFESEH
  , EXCEPTION_SUBCODE_ENGINE_DISPATCHING_ERROR
  , EXCEPTION_SUBCODE_ENGINE_UNWINDING_ERROR
  , EXCEPTION_SUBCODE_ENGINE_CONTINUATION_ERROR
  , EXCEPTION_SUBCODE_NO_CONTINUATION_ADDRESS
  , EXCEPTION_SUBCODE_NO_UNWIND_DATA
  , EXCEPTION_SUBCODE_UNWIND_THROW
  , EXCEPTION_SUBCODE_CCTOR_THROW
  , EXCEPTION_SUBCODE_DTOR_THROW
  , EXCEPTION_SUBCODE_NOEXCEPT_THROW
  , EXCEPTION_SUBCODE_NOEXCEPT_THROW_INLINED
  , EXCEPTION_SUBCODE_ARRAY_UNWIND_DTOR_THROW
  , EXCEPTION_SUBCODE_STD_TERMINATE
  , EXCEPTION_SUBCODE_STD_TERMINATE_INTERNAL
  };

  // custom frame handler return value
  enum
  {
    ExceptionDispatchCPP = 'c++'  // make shure the value is > ::ExceptionCollidedUnwind
  , ExceptionCollisionCPP
  };

}}  // namespace cpprtl::eh


#endif  // include guard

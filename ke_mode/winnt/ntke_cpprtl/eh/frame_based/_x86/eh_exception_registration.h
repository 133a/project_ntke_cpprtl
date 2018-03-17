/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_EXCEPTION_REGISTRATION_H_
#define EH_EXCEPTION_REGISTRATION_H_


#include "eh_config.h"


namespace cpprtl
{
namespace msvc_internal_data
{
namespace eh
{

  struct exception_registration  //  SEH frame data for C++ functions
  {
    enum { FRAME_BASE_DELTA = 0xC };
    static exception_registration const* const REG_END;


    exception_registration* next;  //  the common field for any exception-aware frame;
    void* handler;                 //  the common field for any exception-aware frame - namely '::PEXCEPTION_ROUTINE' but casted to 'void*' here to skip details,
                                   //  for C++ frame do point to the thunk 'mov eax, proper_func_descriptor;  jmp CxxFrameHandler()';
    int   state_id;                //  the C++ specific field.


    static exception_registration* head()
    {
      __asm   mov eax , fs:[0]
    //  return x86.eax -  the current exception registration
    }

    ::size_t frame_pointer() const  //  do NOT use it for any manually implanted registrations
    {
    //  if the exception_record is placed on the stack by a compiler as a part of the function prologue then the following address is known to be the current frame base
      return reinterpret_cast< ::size_t>(this) + sizeof(exception_registration);
    }

    ::size_t const& stack_pointer() const  //  do NOT use it for any manually implanted registrations
    {
    //  if the exception_record is placed on the stack by a compiler as a part of the function prologue then the previous DWORD is known to be the saved x86.esp
      return reinterpret_cast< ::size_t const&>(reinterpret_cast<void* const*>(this)[-1]);
    }

    ::size_t& stack_pointer()  //  do NOT use it for any manually implanted registrations
    {
      return reinterpret_cast< ::size_t&>(reinterpret_cast<void**>(this)[-1]);
    }


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4733)    //  inline asm assigning to 'fs:[0]' : handler not registered as safe handler
#endif //_MSC_VER

    void link()
    {
      __asm  //  x86.ecx = (exception_registration*)this
      {
        mov   eax , fs:[0]
        mov   [ecx].next , eax
        mov   fs:[0] , ecx
      }
    }


    void unlink()
    {
      __asm  //  x86.ecx = (exception_registration*)this
      {
        mov   eax , [ecx].next
        mov   fs:[0] , eax
      }
    }

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER


    protected:
      exception_registration() 
      {}

    private:
      static void* operator new(::size_t);  //  this data is considered as stack-residing

  };


}  // namespace eh
}  // namespace msvc_internal_data
}  // namespace cpprtl


#endif  // include guard


#include "eh_config.h"
#include "eh_framework.h"
#include "eh_exception_code.h"
#include "eh_aux.h"
#include "eh_seh_reg.h"


#ifdef CPPRTL_EH_CFG_SAFESEH
extern "C" IMAGE_LOAD_CONFIG_DIRECTORY const _load_config_used;  // NT_KERNEL_MODE ntoskrnl.lib
extern "C" CHAR const __ImageBase;
#endif


namespace
{
#ifdef NT_KERNEL_MODE
  void get_stack_limits(size_t& top, size_t& base)
  {
    ULONG_PTR low = 0, high = 0;
  #if !(NTDDI_VERSION >= 0x05020000)  // <nt5.2, TODO consider runtime check
    if ( KeGetCurrentIrql() < DISPATCH_LEVEL )
  #endif
    {
      // <=nt5.1 IRQL<DISPATCH_LEVEL, DPC-stack unaware
      // >=nt5.2 any IRQL, DPC-stack aware
      IoGetStackLimits(&low, &high);
    }
    top = low;
    base = high;
  }
#else
  __declspec(naked)
  void __fastcall get_stack_limits(size_t& top, size_t& base)
  {
    __asm  // ecx=&top, edx=&base
    {
      mov  eax , fs:[4]  // NT_TIB.StackBase
      mov  [edx] , eax   // to base
      mov  eax , fs:[8]  // NT_TIB.StackLimit
      mov  [ecx] , eax   // to top
      ret
    }
  }
#endif  // NT_KERNEL_MODE
}  // namespace


namespace cpprtl { namespace eh { namespace msvc_data { namespace seh
{
  void registration::verify() const
  {
    {
      size_t const reg_addr = reinterpret_cast<size_t>(this);

      enum { MISALIGNMENT = (1 << 2) - 1 };  // x86
      if
      (
        (reg_addr & MISALIGNMENT)  // properly aligned
        || !(this < next)          // ordered
        || !frame_handler          // avoid the empty handler
      )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_BAD_REGISTRATION);
      }

      // is on stack
      size_t low = 0, high = 0;
      get_stack_limits(low, high);
      if ( low && high )  // check if stack limits are available
      {
        if ( reg_addr < low || reg_addr > high )  // TODO disregarding sizeof(*this)
        {
          aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_BAD_REGISTRATION_STACK);
        }
      }
    }

    #ifdef CPPRTL_EH_CFG_SAFESEH
    {
    // SafeSEH, NOTE `_load_config_used' contains handlers for the current module only
      bool safe = false;
      if ( rva_t const* const safeseh_table = reinterpret_cast<rva_t const*>(_load_config_used.SEHandlerTable) )  // sorted array
      {
        rva_t const handler_rva = reinterpret_cast<size_t>(frame_handler) - reinterpret_cast<size_t>(&__ImageBase);
        size_t low = 0;
        size_t high = _load_config_used.SEHandlerCount;
        while ( high >= low )
        {
          size_t const mid = (low + high) >> 1;
          if ( handler_rva > safeseh_table[mid] )
          {
            low = mid + 1;
          }
          else if ( handler_rva < safeseh_table[mid] )
          {
            high = mid - 1;
          }
          else
          {
            safe = true;
            break;
          }
        }
      }
      if ( !safe )
      {
        aux_::terminate(EXCEPTION_CODE_CPP, EXCEPTION_SUBCODE_BAD_REGISTRATION_SAFESEH);
      }
    }
    #endif  // CPPRTL_EH_CFG_SAFESEH
  }
}}}}  // namespace cpprtl::eh::msvc_data::seh

/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_MSVC_INTERNAL_DATA_H_
#define EH_MSVC_INTERNAL_DATA_H_


#include "eh_config.h"
#include "eh_msvc_internal_data_const.h"


namespace cpprtl
{
namespace msvc_internal_data
{

////////////////////////////////////////////////////////
////////  MSVC compiler internal stuff
////////////////////////////////////////////////////////
namespace eh
{

  typedef /*::EXCEPTION_DISPOSITION*/ unsigned int (__cdecl *forward_compat_ft)(...);

  namespace aux_
  {
    struct obj {};
  }
  typedef void (aux_::obj::*dtor_ft)();
  typedef void (aux_::obj::*cctor_ft)(void*);
  typedef void (aux_::obj::*cctor_vb_ft)(void*, int);


  struct exception_registration;


  namespace aux_
  {
    struct continuation
    {
      static void invoke(continuation* const, exception_registration const* const);
    };
  }
  typedef aux_::continuation* continuation_ft;


  namespace aux_
  {
    struct funclet
    {
      static continuation_ft invoke(funclet* const, exception_registration const* const);
    };
  }
  typedef aux_::funclet* unwind_action_ft;
  typedef aux_::funclet* catch_handler_ft;


  ////////////////////////////////////////////////////
  ////  msvc's ::type_info raw look-alike

  struct type_descriptor
  {
    void const*    vtable_ptr;          // vtable of type_info class
    char*          unmangled_name;      // used by msvcrt to keep the demangled name returned by type_info::name()
    char           name;                // mangled type name, e.g. ".H" = "int", ".?AUA@@" = "struct A", ".?AVA@@" = "class A" (char[])
  };


  ////////////////////////////////////////////////////
  //// throwed exception stuff

  struct subtype_cast_info
  {
    int  subtype_offset;        // member offset
    int  vbase_table_offset;    // offset of the vbtable (-1 if not a virtual base)
    int  vbase_disp_offset;     // offset to the displacement value inside the vbtable
  };
           

  struct catchable_type
  {
    unsigned int        attributes;    // 0x01: simple type (can be copied by memmove), 0x02: can be caught by reference only, 0x04: has virtual bases
    type_descriptor*    type_descr;    // -->type_descriptor*
    subtype_cast_info   cast_info;     // how to cast the thrown object to this type
    unsigned int        size;          // object size
    union
    {
      cctor_ft          cctor;         // copy constructor address (-->cctor_ft)
      cctor_vb_ft       cctor_vb;      // copy constructor address (-->cctor_vb_ft)
    };
  };


  struct catchable_type_table
  {
    unsigned int       type_array_size;          // number of entries in the type array 
    catchable_type*    catchable_type_array[1];  // -->catchable_type* ( [1] -just a fake size)
  };


  struct exception_descriptor
  {
    unsigned int             attributes;          // 0x01: const, 0x02: volatile
    dtor_ft                  dtor;                // exception destructor (-->dtor_ft)
    forward_compat_ft        forward_compat;      // forward compatibility handler (-->forward_compat_ft)
    catchable_type_table*    catchable_types;     // list of types that can catch this exception (the actual type and all its ancestors) (-->catchable_type_table*)
  };


  ////////////////////////////////////////////////////
  //// function scope stuff (try-catch and unwind information)

  struct unwind_descriptor
  {
    int                 prev_state;      // target state
    unwind_action_ft    action;          // action to perform (unwind funclet address) (-->unwind_action_ft)
  };


  struct catch_descriptor
  {
    unsigned int        attributes;         // 0x01: const, 0x02: volatile, 0x08: reference
    type_descriptor*    type_descr;         // RTTI descriptor of the exception type. 0=(...) (-->type_descriptor*)
    int                 exc_offset;         // ebp-based offset of the exception object in the function stack.  0 = no object (catch by type)
    catch_handler_ft    handler_address;    // address of the catch handler code, returns address where to continues execution (i.e. code after the try block) (-->void*)
  };


  struct try_descriptor
  {
    int                  low_level;           // this try{} covers states ranging from low_level 
    int                  high_level;          // to high_level
    int                  catch_level;         // highest state inside catch handlers of this try
    unsigned int         catch_array_size;    // number of catch handlers
    catch_descriptor*    catch_array;         // catch handlers table (-->catch_descriptor*)
  };


  struct func_descriptor
  {
    unsigned int          magic_number;               // compiler version. 0x19930520: up to VC6, 0x19930521: VC7.x, 0x19930522: VC8
    unsigned int          unwind_array_size;          // number of entries in unwind table
    unwind_descriptor*    unwind_array;               // table of unwind destructors (-->unwind_descriptor*)
    unsigned int          try_array_size;             // number of try blocks in the function
    try_descriptor*       try_array;                  // mapping of catch blocks to try blocks (-->try_descriptor*)
    unsigned int          ip2state_array_size;        // not used on x86
    void*                 ip2state_array;             // not used on x86 (-->ip2state_descriptor* for x64)
    void*                 expected_exceptions_table;  // VC7+ only, expected exceptions list (function "throw" specifier). not enabled in MSVC by default - use /d1ESrt to enable (-->void*)
    unsigned int          flags;                      // VC8+ only, bit 0 set if function was compiled with /EHs
  };


}  //  namespace eh
}  //  namespace msvc_internal_data
}  //  namespace cpprtl


#endif // include guard


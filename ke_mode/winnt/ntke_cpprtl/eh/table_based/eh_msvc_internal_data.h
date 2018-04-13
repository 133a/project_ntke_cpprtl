/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
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

  typedef /*::EXCEPTION_DISPOSITION*/ unsigned int (*forward_compat_ft)(...);


  typedef void (*dtor_ft)(void*);
  typedef void (*cctor_ft)(void*, void*);
  typedef void (*cctor_vb_ft)(void*, void*, int);


#if defined (_M_X64) || defined (_M_AMD64)
  typedef void  (*unwind_action_ft)(void*, void*);
  typedef void* (*catch_handler_ft)(void*, void*);
#elif defined (_M_ARM) || defined (_M_ARM64)
  typedef void  (*unwind_action_ft)(void);
  typedef void* (*catch_handler_ft)(void);
#else
# error check $(target.arch)
#endif


  ////////////////////////////////////////////////////
  ////  msvc's ::type_info raw look-alike
  ////////////////////////////////////////////////////

  struct type_descriptor
  {
    void const*  vtable_ptr;     // vtable of type_info class
    char*        unmangled_name; // used by msvcrt to keep a demangled name returned by type_info::name()
    char         name;           // the actual type is char[], keeps a zero-terminated mangled type name, e.g. ".H" = "int", ".?AUA@@" = "struct A", ".?AVA@@" = "class A"
  };


  ////////////////////////////////////////////////////
  //// throwed exception stuff
  ////////////////////////////////////////////////////

  struct subtype_cast_info
  {
    int  subtype_offset;      // member offset
    int  vbase_table_offset;  // offset of the vbtable (-1 if not a virtual base)
    int  vbase_disp_offset;   // offset to the displacement value inside the vbtable
  };


  struct catchable_type
  {
    unsigned int       attributes;  // 0x01: simple type (can be copied by memmove), 0x02: can be caught by reference only, 0x04: has virtual bases
    rva_t              type_descr;  // -->type_descriptor*
    subtype_cast_info  cast_info;   // how to cast the thrown object to this type
    unsigned int       size;        // object size
    union
    {
      rva_t            cctor;       // copy constructor address (-->cctor_ft)
      rva_t            cctor_vb;    // copy constructor address (-->cctor_vb_ft)
    };
  };


  struct catchable_type_table
  {
    unsigned int  type_array_size;          // number of entries in the type array 
    rva_t         catchable_type_array[1];  // -->catchable_type* ( [1] - it's just a fake size)
  };


  struct exception_descriptor
  {
    unsigned int  attributes;       // 0x01: const, 0x02: volatile
    rva_t         dtor;             // exception destructor (-->dtor_ft)
    rva_t         forward_compat;   // forward compatibility handler (-->forward_compat_ft)
    rva_t         catchable_types;  // list of types that can catch this exception (the actual type and all its ancestors) (-->catchable_type_table*)
  };


  ////////////////////////////////////////////////////
  //// function scope stuff (try-catch and unwind information)
  ////////////////////////////////////////////////////

  struct unwind_descriptor
  {
    int    prev_state;  // target state
    rva_t  action;      // action to perform (unwind funclet address) (-->unwind_action_ft)
  };


  struct ip2state_descriptor
  {
    rva_t  ip;
    int    state;
  };


  struct try_descriptor
  {
    int           low_level;         // this try{} covers states ranging from low_level 
    int           high_level;        // to high_level
    int           catch_level;       // highest state inside catch handlers of this try
    unsigned int  catch_array_size;  // number of catch handlers
    rva_t         catch_array;       // catch handlers table (-->catch_descriptor*)
  };


  struct catch_descriptor
  {
    unsigned int  attributes;       // 0x01: const, 0x02: volatile, 0x08: reference
    rva_t         type_descr;       // RTTI descriptor of the exception type. 0=(...) (-->type_descriptor*)
    int           exc_offset;       // ebp-based offset of the exception object in the function stack.  0 = no object (catch by type)
    rva_t         handler_address;  // address of the catch handler code, returns address where to continues execution (i.e. code after the try block) (-->void*)
#if defined (_M_AMD64) || defined (_M_X64)
    unsigned int  frame_offset;     // x64 specific
#endif
  };


  struct func_descriptor
  {
    unsigned int  magic_number;               // compiler version, 0x19930520: up to msvc6, 0x19930521: msvc2002 msvc2003, 0x19930522: msvc2005
    unsigned int  unwind_array_size;          // number of entries in unwind table
    rva_t         unwind_array;               // table of unwind destructors (-->unwind_descriptor*)
    unsigned int  try_array_size;             // number of try blocks in the function
    rva_t         try_array;                  // mapping of catch blocks to try blocks (-->try_descriptor*)
    unsigned int  ip2state_array_size;        // 
    rva_t         ip2state_array;             // (-->ip2state_descriptor*)
    rva_t         frame_ptrs;                 // (-->???)
    rva_t         expected_exceptions_table;  // >=msvc2002, expected exceptions list (function "throw" specifier). not enabled in MSVC by default - use /d1ESrt to enable (-->void*)
    unsigned int  flags;                      // >=msvc2005, bit 0 set if function was compiled with /EHs
  };


}  // namespace eh
}  // namespace msvc_internal_data
}  // namespace cpprtl


#endif  // include guard


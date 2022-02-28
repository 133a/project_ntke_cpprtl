//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_DATA_H_
#define EH_DATA_H_


#include "eh_config.h"
#include "eh_data_common.h"


//====================
// msvc internal stuff
//--------------------
namespace cpprtl { namespace eh { namespace msvc_data
{
  typedef /*::EXCEPTION_DISPOSITION*/ unsigned (__cdecl *forward_compat_ft)(...);


  struct object_type {};
  typedef void (object_type::*dtor_ft)();
  typedef void (object_type::*cctor_ft)(object_type*);
  typedef void (object_type::*cctor_vb_ft)(object_type*, int);


  namespace continuation
  {
    struct type;
    __declspec(noreturn) void __fastcall start(type*, size_t frame_base, size_t stack_pointer);
  };
  typedef continuation::type* continuation_ft;

  namespace funclet
  {
    struct type;
    continuation_ft __fastcall run(type*, size_t frame_base);
  };
  typedef funclet::type* funclet_ft;


//=======================
// exception object stuff
//-----------------------
  struct catchable_type_descriptor
  {
    unsigned           attributes;  // EXCOBJECT_ enum
    type_descriptor*   type_info;   // exception object/subobject type info
    subtype_cast_info  cast_info;   // cast the exception object to this type
    unsigned           size;        // object size
    union
    {
      cctor_ft         cctor;       // copy constructor address
      cctor_vb_ft      cctor_vb;    // ^
    };
  };


  struct catchable_type_table_descriptor
  {
    unsigned                    catchable_type_array_size;  // number of entries in the type array 
    catchable_type_descriptor*  catchable_type_array[1];    // [VSA]
  };


  struct exception_object_descriptor
  {
    unsigned                          attributes;            // EXCOBJECT_ enum
    dtor_ft                           dtor;                  // exception object destructor
    forward_compat_ft                 forward_compat;        // forward compatibility handler
    catchable_type_table_descriptor*  catchable_type_table;  // list of types that can catch this exception (the type itself and base types if any)
  };


//=====================
// function scope stuff
//---------------------
  struct unwind_descriptor
  {
    frame_state_t  prev_state;  // previous function state
    funclet_ft     funclet;     // unwind funclet address
  };


  struct catch_block_descriptor
  {
    unsigned          attributes;     // CATCH_ enum
    type_descriptor*  type_info;      // catch object type info, 0=(...)
    int               object_offset;  // ebp-based offset of the catch object in the function frame, 0 means no object (catch by type)
    funclet_ft        funclet;        // address of the catch handler code returning a continuation address
  };


  struct try_block_descriptor
  {
    frame_state_t            low_level;               // try{} covers states ranging from low_level 
    frame_state_t            high_level;              // to high_level
    frame_state_t            catch_level;             // highest state inside catch{} blocks of this try
    unsigned                 catch_block_array_size;  // number of catch blocks
    catch_block_descriptor*  catch_block_array;       // catch block array
  };


  struct function_descriptor
  {
    unsigned               signature;                  // compiler identifier, MARK_VC_ enum
    unsigned               unwind_array_size;          // number of entries in unwind table
    unwind_descriptor*     unwind_array;               // unwind funclet table
    unsigned               try_block_array_size;       // number of try blocks in the function
    try_block_descriptor*  try_block_array;            // mapping of catch blocks to try blocks
    unsigned               ip2state_array_size;        // not used on x86
    void*                  ip2state_array;             // not used on x86
    void*                  expected_exceptions_table;  // signature>=MARK_VC7, expected exceptions list ("throw()" specifier), msvc switch -d1ESrt
    unsigned               flags;                      // signature>=MARK_VC8, FUNCTION_ enum
  };

}}}  // namespace cpprtl::eh::msvc_data


#ifdef __ICL
typedef cpprtl::eh::msvc_data::exception_object_descriptor _ThrowInfo;
#endif 


#endif  // include guard

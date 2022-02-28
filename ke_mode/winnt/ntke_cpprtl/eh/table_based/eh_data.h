//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_DATA_H_
#define EH_DATA_H_


#include "eh_config.h"
#include "eh_data_common.h"


namespace cpprtl { namespace eh
{
  namespace aux_
  {
    struct funclet_frame_t;
    struct function_frame_t;
  }
  typedef aux_::funclet_frame_t const*  funclet_frame_t;   // a function or a catch-block-funclet frame
  typedef aux_::function_frame_t const* function_frame_t;  // the function frame
}}  // namespace cpprtl::eh


//====================
// msvc internal stuff
//--------------------
namespace cpprtl { namespace eh { namespace msvc_data
{
  typedef /*::EXCEPTION_DISPOSITION*/ unsigned (*forward_compat_ft)(...);


  struct object_type;
  typedef void (*dtor_ft)(object_type*);
  typedef void (*cctor_ft)(object_type*, object_type const*);
  typedef void (*cctor_vb_ft)(object_type*, object_type const*, int);


#if defined (_M_X64) || defined (_M_AMD64)
  typedef void* (*funclet_ft)(void*, void* function_frame);
#elif defined (_M_ARM) || defined (_M_ARM64)
  typedef void* (*funclet_ft)(void*);
#else
# error check $(target.arch)
#endif


//=======================
// exception object stuff
//-----------------------
  struct catchable_type_descriptor
  {
    unsigned           attributes;  // EXCOBJECT_ enum
    rva_t              type_info;   // exception object/subobject type info -->type_descriptor*
    subtype_cast_info  cast_info;   // cast the exception object to this type
    unsigned           size;        // object size
    union
    {
      rva_t            cctor;       // copy constructor (-->cctor_ft)
      rva_t            cctor_vb;    // ^                (-->cctor_vb_ft)
    };
  };


  struct catchable_type_table_descriptor
  {
    unsigned  catchable_type_array_size;  // number of entries in the type array 
    rva_t     catchable_type_array[1];    // -->(catchable_type_descriptor*)[VSA]
  };


  struct exception_object_descriptor
  {
    unsigned  attributes;            // EXCOBJECT_ enum
    rva_t     dtor;                  // exception object destructor (-->dtor_ft)
    rva_t     forward_compat;        // forward compatibility handler (-->forward_compat_ft)
    rva_t     catchable_type_table;  // list of types that can catch this exception (-->catchable_type_table_descriptor*)
  };


//=====================
// function scope stuff
//---------------------
  struct catch_block_descriptor
  {
    unsigned  attributes;     // CATCH_ enum
    rva_t     type_info;      // catch object type info, 0=(...) (-->type_descriptor*)
    int       object_offset;  // rsp-based offset of the catch object in the function frame, 0 means no object (catch by type)
    rva_t     funclet;        // address of the catch handler code returning a continuation address (-->funclet_ft)
#if defined (_M_AMD64) || defined (_M_X64)
    int       frame_offset;   // x64 specific: function frame offset
#endif
  };


  struct try_block_descriptor
  {
    frame_state_t  low_level;               // try{} covers states ranging from low_level 
    frame_state_t  high_level;              // to high_level
    frame_state_t  catch_level;             // highest state inside catch{} blocks of this try
    unsigned       catch_block_array_size;  // number of catch blocks
    rva_t          catch_block_array;       // catch block array (-->catch_block_descriptor*)
  };


  struct unwind_descriptor
  {
    frame_state_t  prev_state;  // previous function state
    rva_t          funclet;     // unwind funclet address (-->funclet_ft)
  };


  struct ip2state_descriptor
  {
    rva_t          ip_rva;  // instruction pointer
    frame_state_t  state;   // corresponding function state
  };


  struct function_descriptor
  {
    unsigned  signature;                  // compiler identifier, MARK_VC_ enum
    unsigned  unwind_array_size;          // number of entries in unwind table
    rva_t     unwind_array;               // unwind funclet table (-->unwind_descriptor*)
    unsigned  try_block_array_size;       // number of try blocks in the function
    rva_t     try_block_array;            // mapping of catch blocks to try blocks (-->try_block_descriptor*)
    unsigned  ip2state_array_size;        // function state table size
    rva_t     ip2state_array;             // function state table (-->ip2state_descriptor*)
    int       unwind_data_offset;         // rsp-based offset to unwind_data_t residing in the function frame
    rva_t     expected_exceptions_table;  // signature>=MARK_VC7, expected exceptions list ("throw()" specifier), msvc switch -d1ESrt
    unsigned  flags;                      // signature>=MARK_VC8, FUNCTION_ enum
  };

}}}  // namespace cpprtl::eh::msvc_data


#ifdef __ICL
typedef cpprtl::msvc_data::exception_object_descriptor _ThrowInfo;
#endif 


#endif  // include guard

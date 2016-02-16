/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_ENGINE_H_
#define EH_ENGINE_H_


#include "eh_config.h"
#include "eh_framework_specific_header.h"


namespace cpprtl
{

  namespace msvc_internal_data
  {
    namespace eh
    {
      struct exception_descriptor;  // msvc's ::_ThrowInfo lookalike
    }
  }

  namespace eh
  {

    namespace eh_engine
    {

      namespace aux_
      {
        struct frame_t;
        struct func_frame_t;
      } // namespace aux_
  
      typedef aux_::frame_t* frame_ptr_t;                 // the frame of the throwing entity - the function or the catch-block-funclet
      typedef aux_::func_frame_t* funcframe_ptr_t;        // the function frame


      void throw_exception
      (
        void                                          const* const  exc_object
      , msvc_internal_data::eh::exception_descriptor  const* const  exc_descr
      );


      ::EXCEPTION_DISPOSITION frame_handler3
      (
        ::EXCEPTION_RECORD         *        exc_rec
      , frame_ptr_t           const&        frame
      , ::CONTEXT             const* const  context
      , ::DISPATCHER_CONTEXT  const* const  dc
      );

    }  //  namespace eh_engine

  }  //  namespace eh

}  //  namespace cpprtl


#endif // include guard


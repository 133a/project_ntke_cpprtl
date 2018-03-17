/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
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
      struct exception_descriptor;
      struct func_descriptor;
      struct exception_registration;
    }
  }


  namespace eh
  {

    namespace eh_engine
    {

      using msvc_internal_data::eh::exception_registration;


      void throw_exception
      (
        void                                          const* const  exc_object
      , msvc_internal_data::eh::exception_descriptor  const* const  exc_descr
      );


      ::EXCEPTION_DISPOSITION frame_handler3
      (
        ::EXCEPTION_RECORD                            *        exc_rec
      , exception_registration                        *        exc_reg
      , void                                     const* const  context
      , void                                     const* const  dc
      , msvc_internal_data::eh::func_descriptor  const* const  func_descr
      , exception_registration                        *        cg_reg       = 0
      , int                                                    catch_depth  = 0
      );

    }  // namespace eh_engine

  }  // namespace eh

}  // namespace cpprtl


#endif  // include guard


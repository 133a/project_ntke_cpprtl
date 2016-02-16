/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_MSVC_INTERNAL_DATA_AUX_H_
#define EH_MSVC_INTERNAL_DATA_AUX_H_


#include "eh_config.h"
#include "eh_msvc_internal_data.h"

#include <msvc_internal_data_aux.h>
#include <msvc_internal_data_iterator.h>


namespace cpprtl
{
namespace msvc_internal_data_aux
{

  //////////////////////////////////////////////////////////
  //////////  MSVC EH specific types information
  //////////////////////////////////////////////////////////

  template <typename DST, typename A_SRC, typename SZ_SRC = A_SRC>
  struct cl_info;


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::catchable_type_table
  , msvc_internal_data::eh::exception_descriptor
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::eh::catchable_type_table
      , msvc_internal_data::eh::exception_descriptor
      >
  {
    typedef
      cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::eh::catchable_type_table
      , msvc_internal_data::eh::exception_descriptor
      >
      base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::catchable_type
  , msvc_internal_data::eh::catchable_type_table
  >
    : public cl_info_base_with_array_iterator
      <
        msvc_internal_data::eh::catchable_type
      , msvc_internal_data::eh::catchable_type_table
      >
  {
    typedef cl_info_base_with_array_iterator
      <
        msvc_internal_data::eh::catchable_type
      , msvc_internal_data::eh::catchable_type_table
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::try_descriptor
  , msvc_internal_data::eh::func_descriptor
  >
    : public cl_info_base_with_iterator
      <
        msvc_internal_data::eh::try_descriptor
      , msvc_internal_data::eh::func_descriptor
      >
  {
    typedef cl_info_base_with_iterator
      <
        msvc_internal_data::eh::try_descriptor
      , msvc_internal_data::eh::func_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::unwind_descriptor
  , msvc_internal_data::eh::func_descriptor
  >
    : public cl_info_base_with_iterator
      <
        msvc_internal_data::eh::unwind_descriptor
      , msvc_internal_data::eh::func_descriptor
      >
  {
    typedef cl_info_base_with_iterator
      <
        msvc_internal_data::eh::unwind_descriptor
      , msvc_internal_data::eh::func_descriptor
      >
    base_t;
  };


#ifdef CFG_EH_TABLE_BASED  
  template <>
  struct cl_info
  <
    msvc_internal_data::eh::ip2state_descriptor
  , msvc_internal_data::eh::func_descriptor
  >
    : public cl_info_base_with_iterator
      <
        msvc_internal_data::eh::ip2state_descriptor
      , msvc_internal_data::eh::func_descriptor
      >
  {
    typedef cl_info_base_with_iterator
      <
        msvc_internal_data::eh::ip2state_descriptor
      , msvc_internal_data::eh::func_descriptor
      >
    base_t;
  };
#endif // CFG_EH_TABLE_BASED


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::catch_descriptor
  , msvc_internal_data::eh::try_descriptor
  >
    : public cl_info_base_with_iterator
      <
        msvc_internal_data::eh::catch_descriptor
      , msvc_internal_data::eh::try_descriptor
      >
  {
    typedef cl_info_base_with_iterator
      <
        msvc_internal_data::eh::catch_descriptor
      , msvc_internal_data::eh::try_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::type_descriptor
  , msvc_internal_data::eh::catch_descriptor
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::eh::type_descriptor
      , msvc_internal_data::eh::catch_descriptor
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::eh::type_descriptor
      , msvc_internal_data::eh::catch_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::catch_handler_ft
  , msvc_internal_data::eh::catch_descriptor
  >
    : public cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::catch_handler_ft
      , msvc_internal_data::eh::catch_descriptor
      >
  {
    typedef cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::catch_handler_ft
      , msvc_internal_data::eh::catch_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::type_descriptor
  , msvc_internal_data::eh::catchable_type
  >
    : public cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::eh::type_descriptor
      , msvc_internal_data::eh::catchable_type
      >
  {
    typedef cl_info_base_with_single_value_iterator
      <
        msvc_internal_data::eh::type_descriptor
      , msvc_internal_data::eh::catchable_type
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::unwind_action_ft
  , msvc_internal_data::eh::unwind_descriptor
  >
    : public cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::unwind_action_ft
      , msvc_internal_data::eh::unwind_descriptor
      >
  {
    typedef cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::unwind_action_ft
      , msvc_internal_data::eh::unwind_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::dtor_ft
  , msvc_internal_data::eh::exception_descriptor
  >
    : public cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::dtor_ft
      , msvc_internal_data::eh::exception_descriptor
      >
  {
    typedef cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::dtor_ft
      , msvc_internal_data::eh::exception_descriptor
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::cctor_ft
  , msvc_internal_data::eh::catchable_type
  >
    : public cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::cctor_ft
      , msvc_internal_data::eh::catchable_type
      >
  {
    typedef cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::cctor_ft
      , msvc_internal_data::eh::catchable_type
      >
    base_t;
  };


  template <>
  struct cl_info
  <
    msvc_internal_data::eh::cctor_vb_ft
  , msvc_internal_data::eh::catchable_type
  >
    : public cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::cctor_vb_ft
      , msvc_internal_data::eh::catchable_type
      >
  {
    typedef cl_info_base_with_single_value_iterator_and_raw_dst
      <
        msvc_internal_data::eh::cctor_vb_ft
      , msvc_internal_data::eh::catchable_type
      >
    base_t;
  };

}  //  namespace msvc_internal_data_aux


namespace eh
{

//// eh data type iterators
  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::catchable_type
  , msvc_internal_data::eh::catchable_type_table
  >::fwd_iterator                                catchable_type_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::catchable_type
  , msvc_internal_data::eh::catchable_type_table
  >::rev_iterator                                catchable_type_rev_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::try_descriptor
  , msvc_internal_data::eh::func_descriptor
  >::fwd_iterator                                try_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::try_descriptor
  , msvc_internal_data::eh::func_descriptor
  >::rev_iterator                                try_rev_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::unwind_descriptor
  , msvc_internal_data::eh::func_descriptor
  >::fwd_iterator                                unwind_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::unwind_descriptor
  , msvc_internal_data::eh::func_descriptor
  >::rev_iterator                                unwind_rev_iterator;


#ifdef CFG_EH_TABLE_BASED
  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::ip2state_descriptor
  , msvc_internal_data::eh::func_descriptor
  >::fwd_iterator                                ip2state_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::ip2state_descriptor
  , msvc_internal_data::eh::func_descriptor
  >::rev_iterator                                ip2state_rev_iterator;
#endif  //  CFG_EH_TABLE_BASED


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::catch_descriptor
  , msvc_internal_data::eh::try_descriptor
  >::fwd_iterator                                catch_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::catch_descriptor
  , msvc_internal_data::eh::try_descriptor
  >::rev_iterator                                catch_rev_iterator;


//// eh data type single_value-iterators
  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::type_descriptor
  , msvc_internal_data::eh::catch_descriptor
  >::iterator                                    catch_typeinfo_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::type_descriptor
  , msvc_internal_data::eh::catchable_type
  >::iterator                                    catchable_typeinfo_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::catchable_type_table
  , msvc_internal_data::eh::exception_descriptor
  >::iterator                                    catchable_table_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::dtor_ft
  , msvc_internal_data::eh::exception_descriptor
  >::iterator                                    dtor_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::cctor_ft
  , msvc_internal_data::eh::catchable_type
  >::iterator                                    cctor_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::cctor_vb_ft
  , msvc_internal_data::eh::catchable_type
  >::iterator                                    cctorvb_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::unwind_action_ft
  , msvc_internal_data::eh::unwind_descriptor
  >::iterator                                    unwind_action_iterator;


  typedef
  msvc_internal_data_aux::cl_info
  <
    msvc_internal_data::eh::catch_handler_ft
  , msvc_internal_data::eh::catch_descriptor
  >::iterator                                    catch_handler_iterator;


//// root func_descriptor single-value-iterator
  typedef
  msvc_internal_data_aux::aux_::cl_single_value_iterator
  <
    msvc_internal_data_aux::aux_::iterator_traits<msvc_internal_data::eh::func_descriptor, msvc_internal_data::eh::func_descriptor const*>
  > func_descriptor_iterator;

}  //  namespace eh
}  //  namespace cpprtl


#endif // include guard


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "eh_config.h"
#include "eh_msvc_internal_data.h"
#include "eh_msvc_internal_data_aux.h"


namespace cpprtl
{
namespace msvc_internal_data_aux
{

  //////////////////////////////////////////////////////////
  //////////  MSVC EH specific types information
  //////////////////////////////////////////////////////////
  // build time initialized static values

  template <>
  cl_info<msvc_internal_data::eh::catchable_type_table, msvc_internal_data::eh::exception_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::catchable_type_table, msvc_internal_data::eh::exception_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::exception_descriptor::catchable_types;


  template <>
  cl_info<msvc_internal_data::eh::catchable_type, msvc_internal_data::eh::catchable_type_table>::base_t::ARRAY_OFFSET
  cl_info<msvc_internal_data::eh::catchable_type, msvc_internal_data::eh::catchable_type_table>::base_t::array_offset
  = &msvc_internal_data::eh::catchable_type_table::catchable_type_array;


  template <>
  cl_info<msvc_internal_data::eh::catchable_type, msvc_internal_data::eh::catchable_type_table>::base_t::ARRAY_SIZE_OFFSET
  cl_info<msvc_internal_data::eh::catchable_type, msvc_internal_data::eh::catchable_type_table>::base_t::array_size_offset
  = &msvc_internal_data::eh::catchable_type_table::type_array_size;


  template <>
  cl_info<msvc_internal_data::eh::try_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::try_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::func_descriptor::try_array;

  template <>
  cl_info<msvc_internal_data::eh::try_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::ARRAY_SIZE_OFFSET
  cl_info<msvc_internal_data::eh::try_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::array_size_offset
  = &msvc_internal_data::eh::func_descriptor::try_array_size;


  template <>
  cl_info<msvc_internal_data::eh::unwind_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::unwind_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::func_descriptor::unwind_array;

  template <>
  cl_info<msvc_internal_data::eh::unwind_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::ARRAY_SIZE_OFFSET
  cl_info<msvc_internal_data::eh::unwind_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::array_size_offset
  = &msvc_internal_data::eh::func_descriptor::unwind_array_size;


#ifdef CFG_EH_TABLE_BASED  
  template <>
  cl_info<msvc_internal_data::eh::ip2state_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::ip2state_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::func_descriptor::ip2state_array;


  template <>
  cl_info<msvc_internal_data::eh::ip2state_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::ARRAY_SIZE_OFFSET
  cl_info<msvc_internal_data::eh::ip2state_descriptor, msvc_internal_data::eh::func_descriptor>::base_t::array_size_offset
  = &msvc_internal_data::eh::func_descriptor::ip2state_array_size;
#endif // CFG_EH_TABLE_BASED


  template <>
  cl_info<msvc_internal_data::eh::catch_descriptor, msvc_internal_data::eh::try_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::catch_descriptor, msvc_internal_data::eh::try_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::try_descriptor::catch_array;


  template <>
  cl_info<msvc_internal_data::eh::catch_descriptor, msvc_internal_data::eh::try_descriptor>::base_t::ARRAY_SIZE_OFFSET
  cl_info<msvc_internal_data::eh::catch_descriptor, msvc_internal_data::eh::try_descriptor>::base_t::array_size_offset
  = &msvc_internal_data::eh::try_descriptor::catch_array_size;


  template <>
  cl_info<msvc_internal_data::eh::type_descriptor, msvc_internal_data::eh::catch_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::type_descriptor, msvc_internal_data::eh::catch_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::catch_descriptor::type_descr;


  template <>
  cl_info<msvc_internal_data::eh::catch_handler_ft, msvc_internal_data::eh::catch_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::catch_handler_ft, msvc_internal_data::eh::catch_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::catch_descriptor::handler_address;


  template <>
  cl_info<msvc_internal_data::eh::type_descriptor, msvc_internal_data::eh::catchable_type>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::type_descriptor, msvc_internal_data::eh::catchable_type>::base_t::addr_offset
  = &msvc_internal_data::eh::catchable_type::type_descr;


  template <>
  cl_info<msvc_internal_data::eh::unwind_action_ft, msvc_internal_data::eh::unwind_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::unwind_action_ft, msvc_internal_data::eh::unwind_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::unwind_descriptor::action;


  template <>
  cl_info<msvc_internal_data::eh::dtor_ft, msvc_internal_data::eh::exception_descriptor>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::dtor_ft, msvc_internal_data::eh::exception_descriptor>::base_t::addr_offset
  = &msvc_internal_data::eh::exception_descriptor::dtor;


  template <>
  cl_info<msvc_internal_data::eh::cctor_ft, msvc_internal_data::eh::catchable_type>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::cctor_ft, msvc_internal_data::eh::catchable_type>::base_t::addr_offset
  = &msvc_internal_data::eh::catchable_type::cctor;


  template <>
  cl_info<msvc_internal_data::eh::cctor_vb_ft, msvc_internal_data::eh::catchable_type>::base_t::ADDR_OFFSET
  cl_info<msvc_internal_data::eh::cctor_vb_ft, msvc_internal_data::eh::catchable_type>::base_t::addr_offset
  = &msvc_internal_data::eh::catchable_type::cctor_vb;


}  //  namespace msvc_internal_data_aux
}  //  namespace cpprtl


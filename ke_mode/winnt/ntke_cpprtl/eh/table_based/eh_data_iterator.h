//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_DATA_ITERATOR_H_
#define EH_DATA_ITERATOR_H_


#include "eh_config.h"
#include "eh_data.h"
#include <msvc_data_iterator.h>


namespace cpprtl { namespace msvc_data_aux
{
  using namespace eh::msvc_data;

  // try_iterator
  typedef iterator<direct<try_block_descriptor> > try_block_iterator;

  template <> struct descriptor2<try_block_descriptor>
  {
    static try_block_descriptor const* data(function_descriptor const& function, imagebase_t const image_base)
    { return rva_cast<try_block_descriptor const*>(function.try_block_array, image_base); }

    static unsigned int size(function_descriptor const& function, imagebase_t)
    { return function.try_block_array_size; }
  };

  // unwind_iterator
  typedef iterator<direct<unwind_descriptor> > unwind_iterator;

  template <> struct descriptor2<unwind_descriptor>
  {
    static unwind_descriptor const* data(function_descriptor const& function, imagebase_t const image_base)
    { return rva_cast<unwind_descriptor const*>(function.unwind_array, image_base); }

    static unsigned int size(function_descriptor const& function, imagebase_t)
    { return function.unwind_array_size; }
  };

  // ip2state_iterator
  typedef iterator<direct<ip2state_descriptor> > ip2state_iterator;

  template <> struct descriptor2<ip2state_descriptor>
  {
    static ip2state_descriptor const* data(function_descriptor const& function, imagebase_t const image_base)
    { return rva_cast<ip2state_descriptor const*>(function.ip2state_array, image_base); }

    static unsigned int size(function_descriptor const& function, imagebase_t)
    { return function.ip2state_array_size; }
  };

  // catch_block_iterator
  typedef iterator<direct<catch_block_descriptor> > catch_block_iterator;

  template <> struct descriptor2<catch_block_descriptor>
  {
    static catch_block_descriptor const* data(try_block_descriptor const& try_block, imagebase_t const image_base)
    { return rva_cast<catch_block_descriptor const*>(try_block.catch_block_array, image_base); }

    static unsigned int size(try_block_descriptor const& try_block, imagebase_t)
    { return try_block.catch_block_array_size; }
  };

  // catchable_type_iterator
  typedef iterator<indirect<catchable_type_descriptor> > catchable_type_iterator;

  template <> struct descriptor2<catchable_type_descriptor>
  {
    static rva_t const* data(exception_object_descriptor const& exception_object, imagebase_t const image_base)
    { return rva_cast<catchable_type_table_descriptor const*>(exception_object.catchable_type_table, image_base)->catchable_type_array; }

    static unsigned int size(exception_object_descriptor const& exception_object, imagebase_t const image_base)
    { return rva_cast<catchable_type_table_descriptor const*>(exception_object.catchable_type_table, image_base)->catchable_type_array_size; }
  };

  // type_descriptor_accessor
  typedef accessor<direct<type_descriptor> > type_descriptor_accessor;

  template <> struct descriptor2<type_descriptor>
  {
    static type_descriptor const* data(catch_block_descriptor const& catch_block, imagebase_t const image_base)
    { return rva_cast<type_descriptor const*>(catch_block.type_info, image_base); }

    static type_descriptor const* data(catchable_type_descriptor const& catchable_type, imagebase_t const image_base)
    { return rva_cast<type_descriptor const*>(catchable_type.type_info, image_base); }
  };


  // cctor_accessor
  typedef accessor<value<cctor_ft> > cctor_accessor;

  template <> struct descriptor2<cctor_ft>
  {
    static cctor_ft data(catchable_type_descriptor const& catchable_type, imagebase_t const image_base)
    { return rva_cast<cctor_ft>(catchable_type.cctor, image_base); }
  };

  // cctor_vb_accessor
  typedef accessor<value<cctor_vb_ft> > cctor_vb_accessor;

  template <> struct descriptor2<cctor_vb_ft>
  {
    static cctor_vb_ft data(catchable_type_descriptor const& catchable_type, imagebase_t const image_base)
    { return rva_cast<cctor_vb_ft>(catchable_type.cctor_vb, image_base); }
  };

  // dtor_accessor
  typedef accessor<value<dtor_ft> > dtor_accessor;

  template <> struct descriptor2<dtor_ft>
  {
    static dtor_ft data(exception_object_descriptor const& exception_object, imagebase_t const image_base)
    { return rva_cast<dtor_ft>(exception_object.dtor, image_base); }
  };

  // funclet_accessor
  typedef accessor<value<funclet_ft> > funclet_accessor;

  template <> struct descriptor2<funclet_ft>
  {
    static funclet_ft data(unwind_descriptor const& unwind, imagebase_t const image_base)
    { return rva_cast<funclet_ft>(unwind.funclet, image_base); }

    static funclet_ft data(catch_block_descriptor const& catch_block, imagebase_t const image_base)
    { return rva_cast<funclet_ft>(catch_block.funclet, image_base); }
  };

}}  // namespace cpprtl::msvc_data_aux


namespace cpprtl { namespace eh { namespace msvc_data
{
  using msvc_data_aux::try_block_iterator;
  using msvc_data_aux::unwind_iterator;
  using msvc_data_aux::ip2state_iterator;
  using msvc_data_aux::catch_block_iterator;
  using msvc_data_aux::catchable_type_iterator;
  using msvc_data_aux::type_descriptor_accessor;
  using msvc_data_aux::cctor_accessor;
  using msvc_data_aux::cctor_vb_accessor;
  using msvc_data_aux::dtor_accessor;
  using msvc_data_aux::funclet_accessor;
}}}  // namespace cpprtl::eh::msvc_data


#endif  // include guard

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

  // try_block_iterator
  typedef iterator<direct<try_block_descriptor> > try_block_iterator;

  template <> struct descriptor2<try_block_descriptor>
  {
    static try_block_descriptor const* data(function_descriptor const& function, imagebase_t)
    { return function.try_block_array; }

    static unsigned int size(function_descriptor const& function, imagebase_t)
    { return function.try_block_array_size; }
  };

  // unwind_iterator
  typedef iterator<direct<unwind_descriptor> > unwind_iterator;

  template <> struct descriptor2<unwind_descriptor>
  {
    static unwind_descriptor const* data(function_descriptor const& function, imagebase_t)
    { return function.unwind_array; }

    static unsigned int size(function_descriptor const& function, imagebase_t)
    { return function.unwind_array_size; }
  };

  // catch_block_iterator
  typedef iterator<direct<catch_block_descriptor> > catch_block_iterator;

  template <> struct descriptor2<catch_block_descriptor>
  {
    static catch_block_descriptor const* data(try_block_descriptor const& try_block, imagebase_t)
    { return try_block.catch_block_array; }

    static unsigned int size(try_block_descriptor const& try_block, imagebase_t)
    { return try_block.catch_block_array_size; }
  };

  // catchable_type_iterator
  typedef iterator<indirect<catchable_type_descriptor> > catchable_type_iterator;

  template <> struct descriptor2<catchable_type_descriptor>
  {
    static catchable_type_descriptor const* const* data(exception_object_descriptor const& exception_object, imagebase_t)
    { return exception_object.catchable_type_table->catchable_type_array; }

    static unsigned int size(exception_object_descriptor const& exception_object, imagebase_t)
    { return exception_object.catchable_type_table->catchable_type_array_size; }
  };

}}  // namespace cpprtl::msvc_data_aux


namespace cpprtl { namespace eh { namespace msvc_data
{
  using msvc_data_aux::try_block_iterator;
  using msvc_data_aux::unwind_iterator;
  using msvc_data_aux::catch_block_iterator;
  using msvc_data_aux::catchable_type_iterator;
}}}  // namespace cpprtl::eh::msvc_data


#endif  // include guard

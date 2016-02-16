/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtti_type_info.h"
#include "rtti_aux.h"


bool type_info::operator == (type_info const& rhs) const
{
  return this == &rhs || cpprtl::rtti::aux_::strzcmp(this->name(), rhs.name());
}


char const* type_info::name() const
{
  return &mangled_name;
}


type_info::type_info(type_info const&)
{}


type_info& type_info::operator=(type_info const&)
{
  return *this;
}


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef RTTI_TYPE_INFO_H_
#define RTTI_TYPE_INFO_H_


class type_info
{
  char mutable*  unmangled_name;  // used to keep the demangled name returned by type_info::name()
  char           mangled_name;    // the actual type is char[], keeps a zero-terminated mangled type name, e.g. ".H" = "int", ".?AUA@@" = "struct A", ".?AVA@@" = "class A"

public:
  virtual ~type_info() {}
  type_info() {}

public:
  char const* name() const;
  bool operator == (type_info const& rhs) const;
  bool operator != (type_info const& rhs) const  { return !(*this == rhs); }
  bool before(type_info const& rhs) const;

private:
  type_info(type_info const&);
  type_info& operator=(type_info const&);
};


#endif  // include guard


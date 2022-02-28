//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef RTTI_TYPE_INFO_H_
#define RTTI_TYPE_INFO_H_


class type_info
{
  char mutable*  unmangled_name;   // msvcrt uses to keep the demangled name returned by type_info::name()
  char           mangled_name[1];  // char[VSA], keeps a zero-terminated mangled type name

public:
  virtual ~type_info() {}
  type_info() {}

public:
  char const* name() const;
  bool operator==(type_info const& rhs) const;
  bool operator!=(type_info const& rhs) const { return !(*this == rhs); }
  bool before(type_info const& rhs) const;

private:
  type_info(type_info const&);
  type_info& operator=(type_info const&);
};


#endif  // include guard

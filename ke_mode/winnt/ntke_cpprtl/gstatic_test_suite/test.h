//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef TEST_H_
#define TEST_H_


namespace cpprtl { namespace gstatic { namespace test
{
  class some_type
  {
    int volatile val;
  public:
    some_type(int);
    ~some_type();
    operator int() const;
  };
}}}  // namespace cpprtl::gstatic::test


#endif  // include guard

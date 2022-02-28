//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_TESTS_H_
#define EH_TESTS_H_


namespace cpprtl { namespace eh { namespace test
{
  typedef bool (*test_ft)();

// thread-safe tests
  bool test_0101(); bool test_0102(); bool test_0103(); bool test_0104(); bool test_0105(); bool test_0106(); bool test_0107(); bool test_0108(); bool test_0109(); bool test_0110();
  bool test_0201(); bool test_0202(); bool test_0203(); bool test_0204(); bool test_0205(); bool test_0206(); bool test_0207(); bool test_0208(); bool test_0209(); bool test_0210();
  bool test_0301(); bool test_0302(); bool test_0303(); bool test_0304(); bool test_0305(); bool test_0306(); bool test_0307(); bool test_0308(); bool test_0309(); bool test_0310();
  bool test_0401(); bool test_0402(); bool test_0403(); bool test_0404(); bool test_0405(); bool test_0406(); bool test_0407(); bool test_0408(); bool test_0409(); bool test_0410();
  bool test_0501(); bool test_0502(); bool test_0503(); bool test_0504(); bool test_0505(); bool test_0506(); bool test_0507(); bool test_0508(); bool test_0509(); bool test_0510();
  bool test_0601(); bool test_0602(); bool test_0603(); bool test_0604(); bool test_0605(); bool test_0606(); bool test_0607(); bool test_0608(); bool test_0609(); bool test_0610();
  bool test_0701(); bool test_0702(); bool test_0703(); bool test_0704(); bool test_0705(); bool test_0706(); bool test_0707(); bool test_0708(); bool test_0709(); bool test_0710();
  bool test_0801(); bool test_0802(); bool test_0803(); bool test_0804(); bool test_0805(); bool test_0806(); bool test_0807(); bool test_0808(); bool test_0809(); bool test_0810();
  bool test_0901(); bool test_0902(); bool test_0903(); bool test_0904(); bool test_0905(); bool test_0906(); bool test_0907(); bool test_0908(); bool test_0909(); bool test_0910();

// thread-unsafe tests
  bool test_5101(); bool test_5102(); bool test_5103(); bool test_5104(); bool test_5105(); bool test_5106(); bool test_5107(); bool test_5108(); bool test_5109(); bool test_5110();
  bool test_5201(); bool test_5202(); bool test_5203(); bool test_5204(); bool test_5205(); bool test_5206(); bool test_5207(); bool test_5208(); bool test_5209(); bool test_5210();
}}}  // namespace cpprtl::eh::test


#endif  // include guard

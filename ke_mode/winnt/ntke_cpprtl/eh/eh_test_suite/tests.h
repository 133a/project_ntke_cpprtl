/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TESTS_H_
#define TESTS_H_


namespace cpprtl {
namespace test {
namespace eh
{

  typedef int (test_ft)(void);

// thread-safe tests
  int test00();
  int test01();
  int test02();
  int test03();
  int test04();
  int test05();
  int test06();
  int test07();
  int test08();
  int test09();
  int test10();
  int test11();
  int test12();
  int test13();
  int test14();
  int test15();
  int test16();
  int test17();
  int test18();
  int test19();
  int test20();
  int test21();
  int test22();
  int test23();
  int test24();
  int test25();
  int test26();
  int test27();
  int test28();
  int test29();

// thread-unsafe tests
  int test51();
  int test52();
  int test53();
  int test54();
  int test55();
  int test56();
  int test57();
  int test58();
  int test59();

// user-mode tests (stack-consuming)
  int test100();
  int test101();
  int test102();
  int test103();
  int test104();
  int test105();
  int test106();
  int test107();
  int test108();
  int test109();

}  // namespace eh
}  // namespace test
}  // namespace cpprtl


#endif  // include guard


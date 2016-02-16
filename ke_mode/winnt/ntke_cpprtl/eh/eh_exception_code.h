/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_EXCEPTION_CODE_H_
#define EH_EXCEPTION_CODE_H_


namespace cpprtl
{
namespace eh
{
  //// exception codes
  enum
  {
    EXCEPTION_CODE_MSFTCPP     = 0xE0000000 | 'msc',
    EXCEPTION_CODE_CPP         = 0xE0000000 | 'c++',

    EXCEPTION_CODE_STACKWALKER_UNEXPECTED,
    EXCEPTION_CODE_STACKWALKER_BAD_STACK,
    EXCEPTION_CODE_STACKWALKER_UNEXPECTED_LEAF_ENTRY,
    EXCEPTION_CODE_STACKWALKER_UNEXPECTED_RETURN,
  };


  //// invalid exception subcodes
  enum
  {
    EXCEPTION_SUBCODE_DEFAULT,
    EXCEPTION_SUBCODE_UNWIND_THROW,
    EXCEPTION_SUBCODE_CCTOR_THROW,
    EXCEPTION_SUBCODE_DTOR_THROW,
    EXCEPTION_SUBCODE_ARRAY_UNWIND_DTOR_THROW,
    EXCEPTION_SUBCODE_STD_TERMINATE,
  };

}  //  namespace eh
}  //  namespace cpprtl


#endif  // include guard


/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef EH_STACK_WALKER_H_
#define EH_STACK_WALKER_H_


#include "eh_config.h"
#include "eh_framework_specific_header.h"


namespace cpprtl
{
namespace eh
{
namespace eh_engine
{

  void stack_walk(::EXCEPTION_RECORD& exc_rec);

}  // namespace eh_engine
}  // namespace eh
}  // namespace cpprtl


#endif  // include guard


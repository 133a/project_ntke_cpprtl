//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "context_static.h"


namespace cpprtl { namespace eh { namespace test
{
  int context::ctor_count  = 0;
  int context::cctor_count = 0;
  int context::dtor_count  = 0;
  int context::xtor_count  = 0;
  int context::expected    = 0;
  int context::state       = 0;
}}}  // namespace cpprtl::eh::test

//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtti_config.h"
#include "rtti_aux.h"


namespace cpprtl { namespace rtti { namespace aux_
{
  bool strzeq(char const* ch1, char const* ch2)
  {
    while ( *ch1 || *ch2 )
    {
      if ( *(ch1++) != *(ch2++) )
      {
        return false;
      }
    }
    return true;
  }
}}}  // namespace cpprtl::rtti::aux_

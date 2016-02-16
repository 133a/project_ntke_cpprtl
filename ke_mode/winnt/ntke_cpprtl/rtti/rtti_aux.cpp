/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtti_config.h"
#include "rtti_framework_specific_header.h"
#include "rtti_aux.h"


namespace cpprtl
{
namespace rtti
{
namespace aux_
{

  ////////////////////////////////////////////////////
  ////   compare zero-terminated ansi-strings
  ////   TODO: use kernel ::Rtl???()
  bool strzcmp(char const* ch1, char const* ch2)
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

}  //  namespace aux_
}  //  namespace rtti
}  //  namespace cpprtl


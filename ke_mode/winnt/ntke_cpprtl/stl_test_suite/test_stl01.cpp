/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
////
////  testing the std-headers inclusion
////  MT-safe
/////////////////////////////////////////////


#include <algorithm>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
  // #include <ciso646>         //  TODO yet another dir collision
#include <climits>
#include <clocale>
#include <cmath>
#include <complex>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
  // #include <cwctype>         //  TODO yet another dir collision
#include <deque>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <numeric>          
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#ifdef _STLPORT_VERSION
#  include <hash_map>          //  non-std
#  include <hash_set>          //  non-std
#  include <slist>             //  non-std
#  include <unordered_map>     //  non-std
#  include <unordered_set>     //  non-std
#endif

  // #include <iomanip>         //  ok, turned off by config
  // #include <ios>             //  ok, turned off by config
  // #include <iosfwd>          //  ok, turned off by config
  // #include <iostream>        //  ok, turned off by config
  // #include <istream>         //  ok, turned off by config
  // #include <fstream>         //  ok, turned off by config
  // #include <locale>          //  ok, turned off by config
  // #include <ostream>         //  ok, turned off by config
  // #include <sstream>         //  ok, turned off by config
  // #include <streambuf>       //  ok, turned off by config
  // #include <strstream>       //  ok, turned off by config, deprecated from std

  // #include <pthread_alloc>   //  non-std, requires threads
  // #include <rope>            //  non-std, requires threads
  // #include <type_traits>     //  non-std, does msvc even compile this ?


namespace cpprtl { namespace test { namespace stl
{

  int test_stl01()
  {
    return 0;
  }

}  }  }


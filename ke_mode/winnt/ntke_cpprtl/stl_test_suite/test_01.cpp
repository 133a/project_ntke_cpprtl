//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


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
// #include <ciso646>  // TODO yet another dir collision
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
// #include <cwctype>  // TODO yet another dir collision
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
  #include <hash_map>       // non-std
  #include <hash_set>       // non-std
  #include <slist>          // non-std
  #include <unordered_map>  // non-std
  #include <unordered_set>  // non-std
#endif

// #include <iomanip>    // turned off by config
// #include <ios>        // turned off by config
// #include <iosfwd>     // turned off by config
// #include <iostream>   // turned off by config
// #include <istream>    // turned off by config
// #include <fstream>    // turned off by config
// #include <locale>     // turned off by config
// #include <ostream>    // turned off by config
// #include <sstream>    // turned off by config
// #include <streambuf>  // turned off by config
// #include <strstream>  // turned off by config, deprecated from std

namespace cpprtl { namespace stl { namespace test
{
  bool test_01()
  {
    return true;
  }
}}}  // namespace cpprtl::stl::test

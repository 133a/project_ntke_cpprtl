//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum { UNEXPECTED = 5 };
    int      const int_value   = 101;
    unsigned const uint_value  = 2200;
    char     const char_value  = 'a';
    short    const short_value = 23;
  }

//=======================================
// subsequent try-blocks and catch-blocks
//---------------------------------------
  bool test_0201()
  {
    context ctx(int_value + uint_value + char_value + short_value);
    try
    {
      throw int_value;
    }
    catch (int)
    {
      ctx.state += int_value;
    }
    catch (unsigned)
    {
      ctx.state += uint_value;
    }
    catch (char)
    {
      ctx.state += char_value;
    }
    catch (...)
    {
      ctx.state += UNEXPECTED;
    }

    try
    {
      throw uint_value;
    }
    catch (int)
    {
      ctx.state += int_value;
    }
    catch (unsigned)
    {
      ctx.state += uint_value;
    }
    catch (char)
    {
      ctx.state += char_value;
    }
    catch (...)
    {
      ctx.state += UNEXPECTED;
    }

    try
    {
      throw char_value;
    }
    catch (int)
    {
      ctx.state += int_value;
    }
    catch (unsigned)
    {
      ctx.state += uint_value;
    }
    catch (char)
    {
      ctx.state += char_value;
    }
    catch (...)
    {
      ctx.state += UNEXPECTED;
    }

    try
    {
      throw short_value;
    }
    catch (int)
    {
      ctx.state += int_value;
    }
    catch (unsigned)
    {
      ctx.state += uint_value;
    }
    catch (char)
    {
      ctx.state += char_value;
    }
    catch (...)
    {
      ctx.state += short_value;
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

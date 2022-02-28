//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


// nested try blocks tests
// MT-safe
#include "context.h"


namespace cpprtl { namespace eh { namespace test
{
  namespace
  {
    enum
    {
      THROW        = 53
    , UNEXPECTED_1 = 11
    , UNEXPECTED_2 = 13
    , UNEXPECTED_3 = 17
    , E3_VAL       = 3
    , E5_VAL       = 5
    };
    typedef xtor_counter<0> UDT;
    typedef xtor_counter<THROW> EXC;
    typedef xtor_counter<E3_VAL> E3;
    typedef xtor_counter<E5_VAL> E5;
  }

//==================================
// UDT rethrow in nested catch scope
//----------------------------------
  bool test_0701()
  {
    context ctx(2*THROW);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        try 
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          throw EXC(ctx);
        }
        catch (EXC& exc)
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          ctx.state += exc.val;
          try
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            throw;
          }
          catch (EXC exc)
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            ctx.state += exc.val;
          }
          catch (...)
          {
            ctx.state = UNEXPECTED_3;
          }
        }
        catch (...)
        {
          ctx.state = UNEXPECTED_2;
        }
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_1;
      }
    }
    return ctx.ok();
  }

//=======================================
// UDT nested throw in nested catch scope
//---------------------------------------
  bool test_0702()
  {
    context ctx(E3_VAL + E5_VAL);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        try 
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          throw E3(ctx);
        }
        catch (E3 exc)
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          ctx.state += exc.val;
          try
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            throw E5(ctx);
          }
          catch (E5 exc)
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            ctx.state += exc.val;
          }
          catch (...)
          {
            ctx.state = UNEXPECTED_3;
          }
        }
        catch (...)
        {
          ctx.state = UNEXPECTED_2;
        }
      }
      catch (...)
      {
        ctx.state = UNEXPECTED_1;
      }
    }
    return ctx.ok();
  }

//========================================
// UDT rethrow to nested/nesting try-block
//----------------------------------------
  bool test_0703()
  {
    // NOTE 6 nested exceptions
    context ctx(6*THROW);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        try
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          try
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            try
            {
              UDT udt1(ctx);
              UDT udt2(ctx);
              throw EXC(ctx);
            }
            catch (EXC& exc)
            {
              UDT udt1(ctx);
              UDT udt2(ctx);
              ctx.state += exc.val;
              try
              {
                UDT udt1(ctx);
                UDT udt2(ctx);
                throw;
              }
              catch (EXC exc)
              {
                UDT udt1(ctx);
                UDT udt2(ctx);
                ctx.state += exc.val;
                try
                {
                  UDT udt1(ctx);
                  UDT udt2(ctx);
                  throw;
                }
                catch (EXC const& exc)
                {
                  UDT udt1(ctx);
                  UDT udt2(ctx);
                  ctx.state += exc.val;
                  throw;
                }
              }
            }
          }
          catch (EXC& exc)
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            ctx.state += exc.val;
            throw;
          }
        }
        catch (EXC const exc)
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          ctx.state += exc.val;
          throw;
        }
      }
      catch (EXC const& exc)
      {
        ctx.state += exc.val;
      }
    }
    return ctx.ok();
  }

//=============================================
// UDT nested throw to nested/nesting try-block
//---------------------------------------------
  bool test_0704()
  {
    // NOTE 6 nested exceptions
    context ctx(3*E3_VAL + 3*E5_VAL);
    {
      UDT udt1(ctx);
      UDT udt2(ctx);
      try
      {
        UDT udt1(ctx);
        UDT udt2(ctx);
        try
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          try
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            try
            {
              UDT udt1(ctx);
              UDT udt2(ctx);
              throw E3(ctx);
            }
            catch (E3& exc)
            {
              UDT udt1(ctx);
              UDT udt2(ctx);
              ctx.state += exc.val;
              try
              {
                UDT udt1(ctx);
                UDT udt2(ctx);
                throw E5(ctx);
              }
              catch (E5 exc)
              {
                UDT udt1(ctx);
                UDT udt2(ctx);
                ctx.state += exc.val;
                try
                {
                  UDT udt1(ctx);
                  UDT udt2(ctx);
                  throw E3(ctx);
                }
                catch (E3 const& exc)
                {
                  UDT udt1(ctx);
                  UDT udt2(ctx);
                  ctx.state += exc.val;
                  throw E5(ctx);
                }
              }
            }
          }
          catch (E5& exc)
          {
            UDT udt1(ctx);
            UDT udt2(ctx);
            ctx.state += exc.val;
            throw E3(ctx);
          }
        }
        catch (E3 const exc)
        {
          UDT udt1(ctx);
          UDT udt2(ctx);
          ctx.state += exc.val;
          throw E5(ctx);
        }
      }
      catch (E5 const& exc)
      {
        ctx.state += exc.val;
      }
    }
    return ctx.ok();
  }

}}}  // namespace cpprtl::eh::test

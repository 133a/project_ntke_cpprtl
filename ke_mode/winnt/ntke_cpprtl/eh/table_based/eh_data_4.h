//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef EH_DATA_4_H_
#define EH_DATA_4_H_


// FH4 appeared at {msvc2019/ewdk18362}-14.20.27508-x64; turn on by switch `cl -d2FH4', `link -d2:-FH4'
// FH4 became deafault at {msvc2019/ewdk19041}-14.27.29110-x64; turn off by switch `cl -d2FH4-', `link -d2:-FH4-'
// TODO arm64 is expected to add FH4 support while looks like x86 and arm will remain FH3 only
#include "eh_config.h"
#include "eh_data.h"


namespace cpprtl { namespace eh { namespace msvc_data { namespace fh4
{
  namespace packed
  {
    struct ip2state_table;
    struct ip2state_data;
    // uint32   : array size N
    // [N]
    //   uint32 : IP, funclet entry relative, previous value based
    //   uint32 : frame_state+1

    struct funclet2state_table;
    struct funclet2state_data;
    // uint32  : array size N
    // [N]
    //   int32 : rva(funclet_entry)
    //   int32 : rva(ip2state_table)

    struct try_block_table;
    struct try_block_data;
    // uint32   : array size N
    // [N]
    //   uint32 : try_low
    //   uint32 : try_high
    //   uint32 : catch_level
    //   int32  : rva(catch_block_table)

    struct catch_block_table;
    struct catch_block_data;
    // uint32        : array size N
    // [N]
    //   uint8       : flags
    //   uint32      : attributes, exists if(flags.has_attributes)
    //   int32       : rva(type_descriptor), exists if(flags.has_type_info)
    //   uint32      : catch object frame offset, exists if(flags.has_catch_object)
    //   int32       : rva(funclet_ft), catch block entry point
    //   UNION       : 0 <= M <= 2, M = flags.continuation_count
    //     int32[M]  : rva(funclet_ft) if(flags.is_continuation_rva)
    //     uint32[M] : ep_based(funclet_ft) if(!flags.is_continuation_rva)

    struct unwind_table;
    struct unwind_data;
    // uint32   : array size N
    // [N]
    //   uint32 : (prev_offset<<2)|action_type
    //   int32  : rva(funclet_ft) if(!action_type.is_empty)
    //   uint32 : object frame-based offset if(action_type.object_direct_offset), 
    //            frame-based offset of pointer to object if(action_type.object_indirect_offset)

    struct function_data;
    // uint8  : flags
    // uint32 : exists if(flags.has_bbt)
    // int32  : rva(unwind_table), exists if(flags.has_unwind_table)
    // int32  : rva(try_block_table), exists if(flags.has_try_blocks)
    // int32  : rva(ip2state_table) if(!flags.is_multi_funclet), rva(funclet2state_table) otherwise
    // uint32 : x64 specific, rsp-based offset of the stored function frame, exists if(flags.is_catch_funclet)


    class cursor  // packed data accessor
    {
      uint8_t const* pdata;

    public:
      explicit cursor(void const* const p) : pdata ( reinterpret_cast<uint8_t const*>(p) ) {}
      void reset(void const* const p) { pdata = reinterpret_cast<uint8_t const*>(p); }
      uint32_t uint32 ();
      int32_t  int32  ();
      uint8_t  uint8  () { return *(pdata++); }
      template <typename T> T capture() const { return reinterpret_cast<T>(pdata); }
    };
  }  // namespace packed

  typedef packed::ip2state_table      const* ip2state_table_packed;
  typedef packed::ip2state_data       const* ip2state_data_packed;
  typedef packed::funclet2state_table const* funclet2state_table_packed;
  typedef packed::funclet2state_data  const* funclet2state_data_packed;
  typedef packed::try_block_table     const* try_block_table_packed;
  typedef packed::try_block_data      const* try_block_data_packed;
  typedef packed::catch_block_table   const* catch_block_table_packed;
  typedef packed::catch_block_data    const* catch_block_data_packed;
  typedef packed::unwind_table        const* unwind_table_packed;
  typedef packed::unwind_data         const* unwind_data_packed;
  typedef packed::function_data       const* function_data_packed;


  template <typename T>
  T rva_cast(rva_t const rva, imagebase_t const image_base)
  {
    if ( rva )
    {
      return reinterpret_cast<T>(size_t(image_base + rva));
    }
    return 0;
  }


  template <typename DSC>
  class fwd_index
  {
    typedef typename DSC::descriptor_type   descriptor_type;
    typedef typename DSC::data_packed_type  data_packed_type;
    typedef typename DSC::table_packed_type table_packed_type;

  protected:
    size_t            array_size;
    size_t            array_index;
    packed::cursor    cursor;
    table_packed_type table_packed;
    data_packed_type  data_packed;
    descriptor_type   dsc;

    explicit fwd_index(table_packed_type const table)
      : array_size   ( 0 )
      , array_index  ( -1 )
      , cursor       ( table )
      , table_packed ( table )
      , data_packed  ( 0 )
    {
      if ( table_packed )
      {
        array_size = cursor.uint32();
      }
    }
    void upset()
    {
      array_index = array_size;
    }

  public:
    bool valid() const { return array_index < array_size; }
    bool valid(size_t const idx) const { return idx < array_size; }
    void reset()
    {
      array_size = 0;
      array_index = -1;
      cursor.reset(table_packed);
      data_packed = 0;
      if ( table_packed )
      {
        array_size = cursor.uint32();
        next();
      }
    }
    void next()
    {
      if ( ++array_index < array_size )
      {
        data_packed = cursor.capture<data_packed_type>();
        dsc.unpack(cursor);
      }
      else
      {
        upset();
      }
    }
    descriptor_type const* operator->() const { return &dsc; }
    descriptor_type const& descriptor() const { return dsc; }
    data_packed_type       data()       const { return data_packed; }
    table_packed_type      table()      const { return table_packed; }
  };


  struct ip2state_descriptor
  {
    rva_t         ip_rva;
    frame_state_t state;

    typedef ip2state_descriptor   descriptor_type;
    typedef ip2state_data_packed  data_packed_type;
    typedef ip2state_table_packed table_packed_type;

    ip2state_descriptor(ip2state_data_packed const data, rva_t const prev_ip_rva)
      : ip_rva ( prev_ip_rva )
    {
      packed::cursor c(data);
      unpack(c);
    }

    friend fwd_index<ip2state_descriptor>;
  private:
    ip2state_descriptor() {}

    void unpack(packed::cursor& cursor)
    {
      ip_rva += cursor.uint32();
      state = cursor.uint32();
      state -= 1;
    }
  };

  struct ip2state_iterator : public fwd_index<ip2state_descriptor>
  {
    ip2state_iterator(ip2state_table_packed const table, rva_t const fep_rva)
      : fwd_index<ip2state_descriptor> ( table )
    {
      dsc.ip_rva = fep_rva;
      dsc.state = frame_state::INVALID;
      next();
    }
  };


  struct funclet2state_descriptor
  {
    rva_t                 funclet_entry;
    ip2state_table_packed ip2state_table;
    imagebase_t           image_base;

    typedef funclet2state_descriptor   descriptor_type;
    typedef funclet2state_data_packed  data_packed_type;
    typedef funclet2state_table_packed table_packed_type;

    funclet2state_descriptor(funclet2state_data_packed const data, imagebase_t const ib)
      : image_base ( ib )
    {
      packed::cursor c(data);
      unpack(c);
    }

    friend fwd_index<funclet2state_descriptor>;
  private:
    funclet2state_descriptor() {}

    void unpack(packed::cursor& cursor)
    {
      funclet_entry = cursor.int32();
      ip2state_table = rva_cast<ip2state_table_packed>(cursor.int32(), image_base);
    }
  };

  struct funclet2state_iterator : public fwd_index<funclet2state_descriptor>
  {
    funclet2state_iterator(funclet2state_table_packed const table, imagebase_t const ib)
      : fwd_index<funclet2state_descriptor> ( table )
    {
      dsc.image_base = ib;
      next();
    }
  };


  struct try_block_descriptor
  {
    frame_state_t            low_level;
    frame_state_t            high_level;
    frame_state_t            catch_level;
    catch_block_table_packed catch_block_table;
    imagebase_t              image_base;

    typedef try_block_descriptor   descriptor_type;
    typedef try_block_data_packed  data_packed_type;
    typedef try_block_table_packed table_packed_type;

    try_block_descriptor(try_block_data_packed const data, imagebase_t ib)
      : image_base ( ib )
    {
      packed::cursor c(data);
      unpack(c);
    }

    friend fwd_index<try_block_descriptor>;
  private:
    try_block_descriptor() {}

    void unpack(packed::cursor& cursor)
    {
      low_level = cursor.uint32();
      high_level = cursor.uint32();
      catch_level = cursor.uint32();
      catch_block_table = rva_cast<catch_block_table_packed>(cursor.int32(), image_base);
    }
  };

  struct try_block_iterator : public fwd_index<try_block_descriptor>
  {
    try_block_iterator(try_block_table_packed const table, imagebase_t const ib)
      : fwd_index<try_block_descriptor> ( table )
    {
      dsc.image_base = ib;
      next();
    }
  };


  struct catch_block_descriptor
  {
    enum
    {
      ATTRIBUTES       = 1 << 0
    , TYPE_INFO        = 1 << 1
    , CATCH_OBJECT     = 1 << 2
    , CONTINUATION_RVA = 1 << 3
    , CONTINUATION_COUNT_RSHIFT = 4
    , CONTINUATION_COUNT_MASK   = 3
    , CONTINUATION_ARRAY_LEN    = 2
    };

    unsigned               flags;
    unsigned               attributes;
    type_descriptor const* type_info;
    int                    object_offset;
    funclet_ft             catch_funclet;
    size_t                 continuation[CONTINUATION_ARRAY_LEN];
    rva_t                  funclet_entry;
    imagebase_t            image_base;

    typedef catch_block_descriptor   descriptor_type;
    typedef catch_block_data_packed  data_packed_type;
    typedef catch_block_table_packed table_packed_type;

    catch_block_descriptor(catch_block_data_packed const data, rva_t const fep_rva, imagebase_t const ib)
      : funclet_entry ( fep_rva )
      , image_base    ( ib )
    {
      packed::cursor c(data);
      unpack(c);
    }

    bool has_attributes()         const { return !!(flags & ATTRIBUTES); }
    bool has_type_info()          const { return !!(flags & TYPE_INFO); }
    bool has_catch_object()       const { return !!(flags & CATCH_OBJECT); }
    bool is_continuation_rva()    const { return !!(flags & CONTINUATION_RVA); }
    unsigned continuation_count() const { return (flags >> CONTINUATION_COUNT_RSHIFT & CONTINUATION_COUNT_MASK); }

    friend fwd_index<catch_block_descriptor>;
  private:
    catch_block_descriptor() {}

    void unpack(packed::cursor& cursor)
    {
      flags = cursor.uint8();
      attributes = has_attributes() ? cursor.uint32() : 0;
      type_info = has_type_info() ? rva_cast<type_descriptor const*>(cursor.int32(), image_base) : 0;
      object_offset = has_catch_object() ? cursor.uint32() : 0;
      catch_funclet = rva_cast<funclet_ft>(cursor.int32(), image_base);
      for (size_t idx = 0; idx < CONTINUATION_ARRAY_LEN; ++idx)
      {
        continuation[idx] = 0;
        if ( idx < continuation_count() )
        {
          continuation[idx] = is_continuation_rva()
            ? image_base + cursor.int32()
            : image_base + funclet_entry + cursor.uint32();
        }
      }
    }
  };

  struct catch_block_iterator : public fwd_index<catch_block_descriptor>
  {
    catch_block_iterator(catch_block_table_packed const table, rva_t const fep_rva, imagebase_t const ib)
      : fwd_index<catch_block_descriptor> ( table )
    {
      dsc.funclet_entry = fep_rva;
      dsc.image_base = ib;
      next();
    }
  };


  struct unwind_descriptor
  {
    enum
    {
      NO_ACTION              = 0
    , OBJECT_DIRECT_OFFSET   = 1
    , OBJECT_INDIRECT_OFFSET = 2
    , FUNCLET_RVA            = 3
    , OFFSET_RSHIFT          = 2
    , ACTION_TYPE_MASK       = 3
    };

    unsigned     action_type;
    unsigned     prev_offset;
    funclet_ft   funclet;
    object_type* object;
    size_t       function_frame;
    imagebase_t  image_base;

    typedef unwind_descriptor   descriptor_type;
    typedef unwind_data_packed  data_packed_type;
    typedef unwind_table_packed table_packed_type;

    unwind_descriptor(unwind_data_packed const data, function_frame_t const ff, imagebase_t const ib)
      : function_frame ( reinterpret_cast<size_t>(ff) )
      , image_base     ( ib )
    {
      packed::cursor c(data);
      unpack(c);
    }

    bool object_direct_offset()   const { return OBJECT_DIRECT_OFFSET == action_type; }
    bool object_indirect_offset() const { return OBJECT_INDIRECT_OFFSET == action_type; }
    bool is_empty()               const { return NO_ACTION == action_type; }

    friend fwd_index<unwind_descriptor>;
  private:
    unwind_descriptor() {}

    void unpack(packed::cursor& cursor)
    {
      prev_offset = cursor.uint32();
      action_type = prev_offset & ACTION_TYPE_MASK;
      prev_offset >>= OFFSET_RSHIFT;
      funclet = 0;
      object = 0;
      if ( !is_empty() )
      {
        funclet = rva_cast<funclet_ft>(cursor.int32(), image_base);
        if ( object_direct_offset() )
        {
          // TODO arm/arm64 negative offset is expected
          object = reinterpret_cast<object_type*>(function_frame + cursor.uint32());
        }
        else if ( object_indirect_offset() )
        {
          // TODO arm/arm64 negative offset is expected
          object = reinterpret_cast<object_type*>(*reinterpret_cast<size_t const*>(function_frame + cursor.uint32()));
        }
      }
    }
  };

  class unwind_iterator : public fwd_index<unwind_descriptor>
  {
    unwind_data_packed unwind_data_0;

  public:
    unwind_iterator(unwind_table_packed const table, function_frame_t const ff, imagebase_t const ib)
      : fwd_index<unwind_descriptor> ( table )
      , unwind_data_0                ( cursor.capture<unwind_data_packed>() )
    {
      dsc.image_base = ib;
      dsc.function_frame = reinterpret_cast<size_t>(ff);
      next();
    }

    msvc_data::frame_state_t state() const
    {
      return valid()
        ? static_cast<msvc_data::frame_state_t>(array_index)
        : msvc_data::frame_state::EMPTY;
    }

    void state(msvc_data::frame_state_t const state)
    {
      if ( state > msvc_data::frame_state::EMPTY )
      {
        size_t const idx = state;
        if ( idx > array_index )
        {
          reset();
        }
        while ( idx != array_index && valid() )
        {
          next();
        }
      }
      else
      {
        upset();
      }
    }

    void prev()
    {
      if ( valid() )
      {
        unwind_data_packed const prev_unwind_data = reinterpret_cast<unwind_data_packed>(reinterpret_cast<size_t>(data()) - dsc.prev_offset);
        if ( prev_unwind_data < unwind_data_0 )
        {
          upset();
          return;
        }
        else
        {
          reset();
          while ( data() < prev_unwind_data )
          {
            next();
          }
        }
      }
    }
  };


  class function_descriptor
  {
    enum 
    {
      NO_FLAGS        = 0
    , CATCH_FUNCLET   = 1 << 0
    , MULTI_FUNCLET   = 1 << 1
    , BBT             = 1 << 2
    , UNWIND_TABLE    = 1 << 3
    , TRY_BLOCK_TABLE = 1 << 4
    , EHs             = 1 << 5
    , NOEXCEPT        = 1 << 6
    };

    unsigned               flags;
    unwind_table_packed    unwind_table;
    try_block_table_packed try_block_table;
    ip2state_table_packed  ip2state_table;
  public:
    size_t                 frame;
    rva_t                  entry;
    imagebase_t            image_base;

    function_descriptor(function_data_packed const data, funclet_frame_t const ff, rva_t const fep_rva, imagebase_t const ib)
      : flags           ( NO_FLAGS )
      , unwind_table    ( 0 )
      , try_block_table ( 0 )
      , ip2state_table  ( 0 )
      , frame           ( reinterpret_cast<size_t>(ff) )
      , entry           ( fep_rva )
      , image_base      ( ib )
    {
      packed::cursor cursor(data);
      flags = cursor.uint8();
      if ( has_bbt() )
      {
        (void)cursor.uint32();
      }
      if ( has_unwind_table() )
      {
        unwind_table = rva_cast<unwind_table_packed>(cursor.int32(), image_base);
      }
      if ( has_try_blocks() )
      {
        try_block_table = rva_cast<try_block_table_packed>(cursor.int32(), image_base);
      }
      if ( is_multi_funclet() )
      {
        for
        (
          funclet2state_iterator funclet2state(rva_cast<funclet2state_table_packed>(cursor.int32(), image_base), image_base)
        ; funclet2state.valid()
        ; funclet2state.next()
        )
        {
          if ( entry == funclet2state->funclet_entry )
          {
            ip2state_table = funclet2state->ip2state_table;
            break;
          }
        }
      }
      else
      {
        ip2state_table = rva_cast<ip2state_table_packed>(cursor.int32(), image_base);
      }
      if ( is_catch_funclet() )
      {
      #if defined (_M_AMD64) || defined (_M_X64)
        frame = *reinterpret_cast<size_t const*>(frame + cursor.uint32());
      #elif defined (_M_ARM) || defined (_M_ARM64)
        frame = *reinterpret_cast<size_t const*>(frame);  // saved at cpprtl_eh_run_catch_block frame top
      #endif
      }
    }

    bool is_catch_funclet() const { return !!(flags & CATCH_FUNCLET); }
    bool is_multi_funclet() const { return !!(flags & MULTI_FUNCLET); }
    bool has_bbt()          const { return !!(flags & BBT); }
    bool has_unwind_table() const { return !!(flags & UNWIND_TABLE); }
    bool has_try_blocks()   const { return !!(flags & TRY_BLOCK_TABLE); }
    bool is_noexcept()      const { return !!(flags & NOEXCEPT); }

    operator unwind_table_packed()    const { return unwind_table; }
    operator try_block_table_packed() const { return try_block_table; }
    operator ip2state_table_packed()  const { return ip2state_table; }
    operator function_frame_t()       const { return reinterpret_cast<function_frame_t>(frame); }

    msvc_data::frame_state_t ip2state(rva_t const ip_rva) const
    {
      msvc_data::frame_state_t state = msvc_data::frame_state::EMPTY;
      for ( ip2state_iterator ip2state(ip2state_table, entry); ip2state.valid(); ip2state.next() )
      {
        if ( ip_rva < ip2state->ip_rva )
        {
          break;
        }
        state = ip2state->state;
      }
      return state;
    }
  };

}}}}  // namespace cpprtl::eh::msvc_data::fh4


namespace cpprtl { namespace eh { namespace msvc_data { namespace fh4 { namespace packed
{
  namespace
  {
  	uint8_t const lengths[16] = { 1,  2,  1,  3,  1,  2,  1,  4, 1,  2,  1,  3,  1,  2,  1,  5 };
    uint8_t const shifts[16] =  { 25, 18, 25, 11, 25, 18, 25, 4, 25, 18, 25, 11, 25, 18, 25, 0 };
  }  // namespace

  inline uint32_t cursor::uint32()
  {
    size_t const idx = pdata[0] & 0x0F;
    pdata += lengths[idx];
    uint32_t data = *(reinterpret_cast<uint32_t const*>(pdata - 4));  // NOTE unaligned uint32_t read may occur
    data >>= shifts[idx];
    return data;
  }

  inline int32_t cursor::int32()
  {
    int32_t data = *(reinterpret_cast<int32_t const*>(pdata));  // NOTE unaligned int32_t read may occur
    pdata += 4;
    return data;
  }

}}}}}  // namespace cpprtl::eh::msvc_data::fh4::packed


#endif  // include guard

//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#include "rtti_framework.h"
#include "rtti_config.h"
#include "rtti_data.h"
#include "rtti_data_iterator.h"
#include "rtti_aux.h"
#include <std_incl/typeinfo>


namespace cpprtl { namespace rtti
{
  using namespace msvc_data;

  struct polymorphic_object_type;
  typedef polymorphic_object_type const* pcobj_t;
  typedef polymorphic_object_type*       pobj_t;


  bool operator==(type_descriptor const& lhs, type_descriptor const& rhs)
  {
    return (&lhs == &rhs) || aux_::strzeq(lhs.name, rhs.name);
  }


  size_t pointer_cast(size_t obj_addr, subtype_cast_info const& cast_info)
  {
    if ( cast_info.vbase_table_offset >= 0 )
    {
      obj_addr += cast_info.vbase_table_offset;
      obj_addr += *reinterpret_cast<int const*>(*reinterpret_cast<size_t const*>(obj_addr) + cast_info.vbase_disp_offset);
    }
    obj_addr += cast_info.subtype_offset;
    return obj_addr;
  }

  pcobj_t pointer_cast(pcobj_t const obj_ptr, subtype_cast_info const& cast_info)
  {
    return reinterpret_cast<pcobj_t>(pointer_cast(reinterpret_cast<size_t>(obj_ptr), cast_info));
  }


  extern "C" complete_object_descriptor const* __fastcall cpprtl_rtti_get_co_descriptor(pcobj_t obj);

  class complete_object
  {
    complete_object_descriptor const* dsc;
    size_t                            addr;
    imagebase_t                       ib;

  public:
    explicit complete_object(pcobj_t const obj, bool const get_image_base = true)
      : dsc  ( cpprtl_rtti_get_co_descriptor(obj) )
      , addr ( 0 )
      , ib   ( 0 )
    {
      if ( dsc )
      {
        addr = reinterpret_cast<size_t>(obj) - dsc->offset;
        if ( dsc->cd_offset )
        {
          addr -= *reinterpret_cast<int const*>(reinterpret_cast<size_t>(obj) - dsc->cd_offset);
        }
      #ifdef CPPRTL_RTTI_CFG_RVA
        if ( get_image_base )
        {
          if ( dsc->signature == CO_SIG_V0 )
          { 
            IRQL_CHECK ( <=DISPATCH_LEVEL )  // RtlPcToFileHeader()
            ib = reinterpret_cast<imagebase_t>
            (
              ::RtlPcToFileHeader
              (
                const_cast<complete_object_descriptor*>(dsc)
              , reinterpret_cast<void**>(&ib)
              )
            );
          }
          else
          {
            ib = reinterpret_cast<imagebase_t>(dsc) - dsc->self;
          }
        }
      #endif
      }
    }

    operator pcobj_t() const { return reinterpret_cast<pcobj_t>(addr); }
    size_t                            address()    const { return addr; }
    complete_object_descriptor const& descriptor() const { return *dsc; }
    imagebase_t                       image_base() const { return ib; }
  };


//====================================
// single inheritance hierarchy walker
//------------------------------------
  base_class_descriptor const* dynamic_cast_single_inheritance
  (
    complete_object  const&  co
  , type_descriptor  const&  src_type
  , type_descriptor  const&  dst_type
  )
  {
    // checks for downcast, upcasts are expected to be resolved at compile time
    for
    (
      base_class_iterator dst_bc(*class_hierarchy_accessor(co.descriptor(), co.image_base()), co.image_base())
    ; dst_bc.valid()
    ; dst_bc.next()
    )
    {
      if ( dst_type == *type_descriptor_accessor(*dst_bc, co.image_base()) )
      {
        base_class_iterator src_bc(dst_bc);
        for ( src_bc.next(); src_bc.valid(); src_bc.next() )
        {
          if ( src_bc->attributes & (BC_NOT_PUBLIC_BASE_4 | BC_NOT_PUBLIC_BASE_8) )
          {
            break;
          }
          if ( src_type == *type_descriptor_accessor(*src_bc, co.image_base()) )
          {
            return &*src_bc;
          }
        }
        break;
      }
    }
    return 0;
  }


//======================================
// multiple inheritance hierarchy walker
//--------------------------------------
  namespace mi_aux
  {
    base_class_descriptor const* check_cross_cast
    (
      base_class_iterator  const&  src
    , base_class_iterator  const&  dst
    )
    {
      base_class_descriptor const* bc = &*dst;
      if ( (src->attributes & BC_NOT_VISIBLE) || (dst->attributes & (BC_NOT_VISIBLE | BC_AMBIGUOUS)) )
      {
        bc = 0;
      }
      return bc;
    }

    base_class_descriptor const* check_down_cast
    (
      base_class_iterator  const&  src
    , base_class_iterator  const&  dst
    , imagebase_t          const   image_base
    )
    {
      base_class_descriptor const* bc = &*dst;
      if ( dst->attributes & BC_WITH_HIERARCHY )
      {
        base_class_iterator src_in_dst(*class_hierarchy_accessor(*dst, image_base), image_base);
        if ( src_in_dst[src.index() - dst.index()], src_in_dst.valid() )
        {
          if ( src_in_dst->attributes & BC_NOT_VISIBLE )
          {
            bc = 0;
          }
        }
      }
      else
      {
        if ( (dst.index() == 0) && (src->attributes & BC_NOT_VISIBLE) )
        {
          bc = 0;
        }
      }
      return bc;
    }
  }  // namespace mi_aux

  base_class_descriptor const* dynamic_cast_multiple_inheritance
  (
    complete_object  const&  co
  , size_t           const   src_offset
  , type_descriptor  const&  src_type
  , type_descriptor  const&  dst_type
  )
  {
    base_class_iterator bc(*class_hierarchy_accessor(co.descriptor(), co.image_base()), co.image_base());
    base_class_iterator src = bc;
    base_class_iterator dst = bc;
    src.deface();
    dst.deface();

    for ( ; bc.valid(); bc.next() )
    {
      if ( dst_type == *type_descriptor_accessor(*bc, co.image_base()) )
      {
        if ( src.valid() )  // left-right cross-cast
        {
          return mi_aux::check_cross_cast(src, bc);
        }
        dst = bc;
      }
      if
      (
        (src_type == *type_descriptor_accessor(*bc, co.image_base())) &&
        (co.address() + src_offset == pointer_cast(co.address(), bc->cast_info))
      )
      {
        if ( dst.valid() )
        {
          if ( bc.index() - dst.index() <= dst->nested_bases )  // downcast
          {
            return mi_aux::check_down_cast(bc, dst, co.image_base());
          }
          else  // right-left cross-cast
          {
            return mi_aux::check_cross_cast(bc, dst);
          }
        }
        src = bc;
      }
    }
    return 0;
  }


//=====================================
// virtual inheritance hierarchy walker
//-------------------------------------
  namespace vi_aux
  {
    struct hierarchy_walker_state
    {
      complete_object       const& co;
      base_class_descriptor const* cross_cast_src;
      base_class_descriptor const* cross_cast_dst;
      base_class_descriptor const* down_cast_dst;
      size_t                       down_cast_dst_offset;
      bool                         down_cast_allowed;
      bool                         directly_public;

      explicit hierarchy_walker_state(complete_object const& co)
        : co                   ( co )
        , cross_cast_src       ( 0 )
        , cross_cast_dst       ( 0 )
        , down_cast_dst        ( 0 )
        , down_cast_dst_offset ( 0 )
        , down_cast_allowed    ( true )
        , directly_public      ( false )
      {}

      void check_cross_cast_src(base_class_iterator const& src)
      {
        if ( !(src->attributes & (BC_NOT_VISIBLE | BC_NOT_PUBLIC_BASE_4 | BC_NOT_PUBLIC_BASE_8)) )
        {
          cross_cast_src = &*src;
        }
      }

      void check_cross_cast_dst(base_class_iterator const& dst)
      {
        if ( !(dst->attributes & (BC_NOT_VISIBLE | BC_AMBIGUOUS)) )
        {
          cross_cast_dst = &*dst;
        }
      }

      bool check_down_cast(base_class_iterator const& src, base_class_iterator const& dst)
      {
        if ( down_cast_allowed )
        {
          if ( dst->attributes & BC_WITH_HIERARCHY )
          {
            base_class_iterator src_in_dst(*class_hierarchy_accessor(*dst, co.image_base()), co.image_base());
            if ( src_in_dst[src.index() - dst.index()], src_in_dst.valid() )
            {
              if ( src_in_dst->attributes & BC_NOT_VISIBLE )
              {
                down_cast_allowed = false;
              }
              directly_public = !(src_in_dst->attributes & (BC_NOT_PUBLIC_BASE_4 | BC_NOT_PUBLIC_BASE_8));
            }
          }
          else
          {
            if ( (dst.index() == 0) && (src->attributes & BC_NOT_VISIBLE) )
            {
              down_cast_allowed = false;
            }
            directly_public = true;
          } 
          if ( down_cast_allowed && directly_public )
          {
            size_t const dst_offset = pointer_cast(co.address(), dst->cast_info) - co.address();
            if ( down_cast_dst && (dst_offset != down_cast_dst_offset) )
            {
              return false;  // ambiguous dst
            }
            down_cast_dst = &*dst;
            down_cast_dst_offset = dst_offset;
          }
        }
        return true;
      }
    };
  }  // namespace vi_aux_

  base_class_descriptor const* dynamic_cast_virtual_inheritance
  (
    complete_object  const&  co
  , size_t           const   src_offset
  , type_descriptor  const&  src_type
  , type_descriptor  const&  dst_type
  )
  {
    vi_aux::hierarchy_walker_state walker_state(co);
    base_class_iterator bc(*class_hierarchy_accessor(co.descriptor(), co.image_base()), co.image_base());
    base_class_iterator dst = bc;
    dst.deface();
    for ( ; bc.valid(); bc.next() )
    {
      if ( dst_type == *type_descriptor_accessor(*bc, co.image_base()) )
      {
        dst = bc;
        walker_state.check_cross_cast_dst(dst);  // possible cross-cast destination
      }
      if
      (
        (src_type == *type_descriptor_accessor(*bc, co.image_base())) &&
        (co.address() + src_offset == pointer_cast(co.address(), bc->cast_info))
      )
      {
        if ( dst.valid() && (bc.index() - dst.index() <= dst->nested_bases) )  // downcast
        {
          if ( !walker_state.check_down_cast(bc, dst) )
          {
            return 0;
          }
        }
        else  // cross-cast
        {
          walker_state.check_cross_cast_src(bc);  // possible cross-cast source
        }
      }
    }
    if ( walker_state.down_cast_allowed && walker_state.down_cast_dst )
    {
      return walker_state.down_cast_dst;
    }
    if ( walker_state.cross_cast_src && walker_state.cross_cast_dst )
    {
      return walker_state.cross_cast_dst;
    }
    return 0;
  }


//=========================================================
// msvc internally pre-declared RTTI routines' entry points
//---------------------------------------------------------
  extern "C"
  void* __cdecl __RTDynamicCast
  (
    pcobj_t                 src_obj
  , long            const   vfdelta
  , type_descriptor const&  src_type
  , type_descriptor const&  dst_type
  , bool            const   is_ref
  )
  throw (...)
  {
    pcobj_t dst_obj = 0;
    if ( src_obj )
    {
      complete_object co(src_obj);
      if ( co )
      {
        base_class_descriptor const* bc = 0;
        class_hierarchy_accessor ch(co.descriptor(), co.image_base());
        if ( !(ch->attributes & CH_MULTIPLE_INHERITANCE) )
        {
          bc = dynamic_cast_single_inheritance(co, src_type, dst_type);
        }
        else
        {
          size_t const src_offset = (reinterpret_cast<size_t&>(src_obj) -= vfdelta) - co.address();
          if ( !(ch->attributes & CH_VIRTUAL_INHERITANCE) )
          {
            bc = dynamic_cast_multiple_inheritance(co, src_offset, src_type, dst_type);
          }
          else
          {
            bc = dynamic_cast_virtual_inheritance(co, src_offset, src_type, dst_type);
          }
        }
        if ( bc )
        {
          dst_obj = pointer_cast(co, bc->cast_info);
        }
      }
    }
    if ( is_ref && !dst_obj )
    {
      throw std::bad_cast();
    }
    return const_cast<pobj_t>(dst_obj);
  }


  extern "C"
  void const* __cdecl __RTtypeid(pcobj_t const src_obj)
  throw (...)
  {
    type_descriptor const* type_info = 0;
    if ( src_obj )
    {
      complete_object co(src_obj);
      if ( co )
      {
        type_info = &*type_descriptor_accessor(co.descriptor(), co.image_base());
      }
    }
    if ( !type_info )
    {
      throw std::bad_typeid();
    }
    return type_info;
  }


  extern "C"
  void* __cdecl __RTCastToVoid(pcobj_t const src_obj)
  throw (...)
  {
    pcobj_t dst_obj = 0;
    if ( src_obj )
    {
      dst_obj = complete_object(src_obj, false);
      if ( !dst_obj )
      {
        throw std::bad_cast();
      }
    }
    return const_cast<pobj_t>(dst_obj);
  }

}}  // namespace cpprtl::rtti

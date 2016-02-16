/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#include "rtti_framework_specific_header.h"

#include "rtti_config.h"
#include "rtti_msvc_internal_data.h"
#include "rtti_msvc_internal_data_aux.h"
#include "rtti_engine.h"
#include "rtti_aux.h"


#include <std_incl/typeinfo>


namespace
{

  bool operator==(cpprtl::msvc_internal_data::rtti::type_descriptor const& lhs, cpprtl::msvc_internal_data::rtti::type_descriptor const& rhs)
  {
    return &lhs == &rhs || cpprtl::rtti::aux_::strzcmp(&lhs.name, &rhs.name);
  }


  int exception_access_violation_seh_filter(int const& exc_code)
  {
    return STATUS_ACCESS_VIOLATION == exc_code ? EXCEPTION_EXECUTE_HANDLER: EXCEPTION_CONTINUE_SEARCH;
  }


  cpprtl::image_base_t get_image_base(void const* ptr)
  {
  #ifdef CFG_RVA
    void* ib = 0;
    IRQL_CHECK ( <=DISPATCH_LEVEL )  // RtlPcToFileHeader()
    ib = ::RtlPcToFileHeader(const_cast<void*>(ptr), &ib);
    return reinterpret_cast<cpprtl::image_base_t>(ib);
  #else
    return 0;
  #endif
  }

}


namespace cpprtl
{
namespace rtti
{

  // typedef int bad_cast_t;
  // typedef int bad_typeid_t;
  typedef ::std::bad_cast     bad_cast_t;
  typedef ::std::bad_typeid   bad_typeid_t;


  pcobj_t pointer_cast(pcobj_t const complete_obj, msvc_internal_data::rtti::subtype_cast_info const& cast_info)
  {
    ::size_t ptr = reinterpret_cast< ::size_t>(complete_obj);
    if (cast_info.vbase_table_offset >= 0)
    {
      ptr += cast_info.vbase_table_offset;
      ptr += *reinterpret_cast<int*>(*reinterpret_cast< ::size_t*>(ptr) + cast_info.vbase_disp_offset);
    }
    ptr += cast_info.subtype_offset;
    return reinterpret_cast<pcobj_t>(ptr);
  }


  msvc_internal_data::rtti::complete_object_locator const* get_complete_object_locator(pcobj_t obj)
  {
    return (*reinterpret_cast<msvc_internal_data::rtti::complete_object_locator** const*>(obj))[-1];
  }


  pcobj_t get_complete_object(pcobj_t src_obj)
  {
    msvc_internal_data::rtti::complete_object_locator const* pcol = get_complete_object_locator(src_obj);
    ::size_t complete_obj = reinterpret_cast< ::size_t>(src_obj) - pcol->offset;
    if (pcol->cd_offset)
    {
      complete_obj -= (reinterpret_cast< ::size_t>(src_obj) - pcol->cd_offset);
    }
    return reinterpret_cast<pcobj_t>(complete_obj);
  }


  struct complete_object
  {
    msvc_internal_data::rtti::complete_object_locator const* locator;
    pcobj_t                                                  ptr;

    complete_object(pcobj_t src)
      : locator(get_complete_object_locator(src))
      , ptr(get_complete_object(src))
    {
    }


    bool check() const  //  TODO ?
    {
      return locator && ptr;
    }
  };


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
  ////  single inheritance hierarchy walker
  /////////////////////////////////////////////////////////
  msvc_internal_data::rtti::base_class_descriptor2 const* dynamic_cast_single_inheritance
  (
    complete_object                              const&  complete_obj
  , msvc_internal_data::rtti::type_descriptor    const&  src_type_dsc
  , msvc_internal_data::rtti::type_descriptor    const&  dst_type_dsc
  , ::size_t                                     const&  image_base
  )
  {
    // checks for downcast (base to derived). upcasts are expected to be resolved at compile time
    class_hierarchy ch(complete_obj.locator, image_base);
    for (base_class_iterator dst_bc(*base_class_array(*ch, image_base), *ch, image_base); dst_bc.valid(); dst_bc.next())
    {
      if (dst_type_dsc == **base_class_type_descriptor(*dst_bc, image_base))
      {
        base_class_iterator src_bc(dst_bc);
        for (src_bc.next(); src_bc.valid(); src_bc.next())
        {
          if (src_bc->attributes & msvc_internal_data::rtti::BC_NOT_PUBLIC_BASE)
          {
            break;
          }
          if (src_type_dsc == **base_class_type_descriptor(*src_bc, image_base))
          {
            return *src_bc;
          }
        }
        break;
      }
    }
    return 0;
  }


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
  ////  multiple inheritance hierarchy walker aux staff
  /////////////////////////////////////////////////////////
  namespace mi_aux_
  {

    msvc_internal_data::rtti::base_class_descriptor2 const* check_cross_cast(base_class_iterator const& src, base_class_iterator const& dst)
    {
      msvc_internal_data::rtti::base_class_descriptor2 const* bc_ret = *dst;
      if 
      (
        src->attributes &  msvc_internal_data::rtti::BC_NOT_VISIBLE  ||
        dst->attributes & (msvc_internal_data::rtti::BC_NOT_VISIBLE | msvc_internal_data::rtti::BC_AMBIGUOUS)
      )
      {
        bc_ret = 0;
      }
      return bc_ret;
    }


    msvc_internal_data::rtti::base_class_descriptor2 const* check_down_cast(base_class_iterator const& src, base_class_iterator const& dst)
    {
      msvc_internal_data::rtti::base_class_descriptor2 const* bc_ret = *dst;

      if ( dst->attributes & msvc_internal_data::rtti::BC_WITH_HIERARCHY )  // the real base_class_descriptor2*
      {
        ::size_t const image_base = dst.get_image_base();
        class_hierarchy dst_ch(*dst, image_base);
        base_class_iterator src_in_dst(*base_class_array(*dst_ch, image_base), *dst_ch, image_base);

        if ( src_in_dst[src.index() - dst.index()].valid() )
        {
          if ( src_in_dst->attributes & msvc_internal_data::rtti::BC_NOT_VISIBLE )
          {
            bc_ret = 0;
          }
        }
      }
      else   // our base_class_descriptor2* is actually just an base_class_descriptor*
      {
        if ( dst.index() == 0  &&  (src->attributes & msvc_internal_data::rtti::BC_NOT_VISIBLE) )
        {
          bc_ret = 0;
        }
      }
      return bc_ret;
    }

  }  // namespace mi_aux_


  /////////////////////////////////////////////////////////
  ////  multiple inheritance hierarchy walker
  /////////////////////////////////////////////////////////
  msvc_internal_data::rtti::base_class_descriptor2 const* dynamic_cast_multiple_inheritance
  (
    complete_object                              const&  complete_obj
  , ::size_t                                     const&  src_obj_offset
  , msvc_internal_data::rtti::type_descriptor    const&  src_type_dsc
  , msvc_internal_data::rtti::type_descriptor    const&  dst_type_dsc
  , ::size_t                                     const&  image_base
  )
  {
    class_hierarchy ch(complete_obj.locator, image_base);

    base_class_iterator bc_iter(*base_class_array(*ch, image_base), *ch, image_base);
    base_class_iterator dst_bc = bc_iter;
    base_class_iterator src_bc = bc_iter;
    dst_bc.deface();
    src_bc.deface();

    for ( ; bc_iter.valid(); bc_iter.next() )
    {

      if ( dst_type_dsc == **base_class_type_descriptor(*bc_iter, image_base) )
      {
        if ( src_bc.valid() )  // left-right cross-cast
        {
          return mi_aux_::check_cross_cast(src_bc, bc_iter);
        }
        dst_bc = bc_iter;
      }

      if
      (
          src_type_dsc == **base_class_type_descriptor(*bc_iter, image_base)
      &&  reinterpret_cast< ::size_t>(complete_obj.ptr) + src_obj_offset == reinterpret_cast< ::size_t>(pointer_cast(complete_obj.ptr, bc_iter->cast_info))
      )
      {
        if ( dst_bc.valid() )
        {
          if (bc_iter.index() - dst_bc.index() <= dst_bc->num_contained_bases)  // base-derived cast
          {
            return mi_aux_::check_down_cast(bc_iter, dst_bc);
          }
          else  // right-left cross-cast
          {
            return mi_aux_::check_cross_cast(bc_iter, dst_bc);
          }
        }
        src_bc = bc_iter;
      }
    }
    return 0;  // there was not found src and/or dst types in type hierarchy
  }


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
  //// virtual inheritance hierarchy walker aux staff
  /////////////////////////////////////////////////////////
  namespace vi_aux_
  {

    struct vi_walker_state
    {
      complete_object const& complete_obj;
      bool down_cast_allowed;
      bool directly_public;
      msvc_internal_data::rtti::base_class_descriptor2 const* cross_cast_src;
      msvc_internal_data::rtti::base_class_descriptor2 const* cross_cast_dst;
      msvc_internal_data::rtti::base_class_descriptor2 const* down_cast_dst;
      ::size_t down_cast_dst_offset;

      vi_walker_state(complete_object const& co)
        : complete_obj(co)
        , down_cast_allowed(true)
        , directly_public(false)
        , cross_cast_src(0)
        , cross_cast_dst(0)
        , down_cast_dst(0)
        , down_cast_dst_offset(0)
      {}
    };


    void check_cross_cast_src(base_class_iterator const& src, vi_walker_state& state)
    {
      if ( !(src->attributes & (msvc_internal_data::rtti::BC_NOT_VISIBLE | msvc_internal_data::rtti::BC_NOT_PUBLIC_BASE)) )
      {
        state.cross_cast_src = *src;
      }
    }


    void check_cross_cast_dst(base_class_iterator const& dst, vi_walker_state& state)
    {
      if ( !(dst->attributes & (msvc_internal_data::rtti::BC_NOT_VISIBLE | msvc_internal_data::rtti::BC_AMBIGUOUS)) )
      {
        state.cross_cast_dst = *dst;
      }
    }


    bool check_down_cast(base_class_iterator const& src, base_class_iterator const& dst, vi_walker_state& state)
    {
      if ( state.down_cast_allowed )
      {
        if ( dst->attributes & msvc_internal_data::rtti::BC_WITH_HIERARCHY )  // the real base_class_descriptor2*
        {
          ::size_t const image_base = dst.get_image_base();
          class_hierarchy dst_ch(*dst, image_base);
          base_class_iterator src_in_dst(*base_class_array(*dst_ch, image_base), *dst_ch, image_base);
  
          if ( src_in_dst[src.index() - dst.index()].valid() )
          {
            if ( src_in_dst->attributes & msvc_internal_data::rtti::BC_NOT_VISIBLE )
            {
              state.down_cast_allowed = false;
            }
            state.directly_public = !(src_in_dst->attributes & msvc_internal_data::rtti::BC_NOT_PUBLIC_BASE);
          }
        }
        else   // our base_class_descriptor2* is actually just an base_class_descriptor*
        {
          if ( dst.index() == 0  &&  (src->attributes & msvc_internal_data::rtti::BC_NOT_VISIBLE) )
          {
            state.down_cast_allowed = false;
          }
          state.directly_public = true;
        } 

        if (state.down_cast_allowed && state.directly_public)
        {
          ::size_t const current_dst_offset = reinterpret_cast< ::size_t>(pointer_cast(state.complete_obj.ptr, dst->cast_info)) - reinterpret_cast< ::size_t>(state.complete_obj.ptr);
          if (state.down_cast_dst && current_dst_offset != state.down_cast_dst_offset)
          {
            return false;  // ambiguous dst
          }
          state.down_cast_dst = *dst;
          state.down_cast_dst_offset = current_dst_offset;
        }

      }
      return true;
    }

  }  // namespace vi_aux_


  /////////////////////////////////////////////////////////
  ////  virtual inheritance hierarchy walker
  /////////////////////////////////////////////////////////
  msvc_internal_data::rtti::base_class_descriptor2 const* dynamic_cast_virtual_inheritance
  (
    complete_object                             const&  complete_obj
  , ::size_t                                    const&  src_obj_offset
  , msvc_internal_data::rtti::type_descriptor   const&  src_type_dsc
  , msvc_internal_data::rtti::type_descriptor   const&  dst_type_dsc
  , ::size_t                                    const&  image_base
  )
  {
    vi_aux_::vi_walker_state vi_state(complete_obj);

    class_hierarchy ch(complete_obj.locator, image_base);
    base_class_iterator bc_iter(*base_class_array(*ch, image_base), *ch, image_base);
    base_class_iterator dst_bc = bc_iter;
    dst_bc.deface();

    for ( ; bc_iter.valid(); bc_iter.next() )
    {

      if ( dst_type_dsc == **base_class_type_descriptor(*bc_iter, image_base) )
      {
        dst_bc = bc_iter;
        vi_aux_::check_cross_cast_dst(dst_bc, vi_state);  // possible cross-cast destination
      }

      if
      (
          src_type_dsc == **base_class_type_descriptor(*bc_iter, image_base)
      &&  reinterpret_cast< ::size_t>(complete_obj.ptr) + src_obj_offset == reinterpret_cast< ::size_t>(pointer_cast(complete_obj.ptr, bc_iter->cast_info))
      )
      {
        if ( dst_bc.valid()  &&  bc_iter.index() - dst_bc.index() <= dst_bc->num_contained_bases )   // down-cast
        {
          if ( !vi_aux_::check_down_cast(bc_iter, dst_bc, vi_state) )
          {
            return 0;
          }
        }
        else  // cross-cast
        {
          vi_aux_::check_cross_cast_src(bc_iter, vi_state);  // possible cross-cast source
        }
      }
    }

    if (vi_state.down_cast_allowed && vi_state.down_cast_dst)
    {
      return vi_state.down_cast_dst;
    }

    if (vi_state.cross_cast_src && vi_state.cross_cast_dst)
    {
      return vi_state.cross_cast_dst;
    }
    return 0;
  }


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
  void* dynamic_cast_impl
  (
    pcobj_t src_obj
  , long vfdelta
  , msvc_internal_data::rtti::type_descriptor const& src_type_dsc
  , msvc_internal_data::rtti::type_descriptor const& dst_type_dsc
  , bool is_ref
  )
  {
    pcobj_t dst_obj = 0;

    if (src_obj)
    {
      __try
      {
        complete_object const complete_obj(src_obj);
        if (complete_obj.check())
        {
          reinterpret_cast< ::size_t&>(src_obj) -= vfdelta;
          cpprtl::image_base_t image_base = get_image_base(complete_obj.locator);
          class_hierarchy ch(complete_obj.locator, image_base);
          msvc_internal_data::rtti::base_class_descriptor2 const* base_class = 0;
          ::size_t src_obj_offset = reinterpret_cast< ::size_t>(src_obj) - reinterpret_cast< ::size_t>(complete_obj.ptr);
  
          if ( !(ch->attributes & msvc_internal_data::rtti::CH_MULTIPLE_INHERITANCE) )
          {
            base_class = dynamic_cast_single_inheritance(complete_obj, src_type_dsc, dst_type_dsc, image_base);
          }
          else if ( !(ch->attributes & msvc_internal_data::rtti::CH_VIRTUAL_INHERITANCE) )
          {
            base_class = dynamic_cast_multiple_inheritance(complete_obj, src_obj_offset, src_type_dsc, dst_type_dsc, image_base);
          }
          else
          {
            base_class = dynamic_cast_virtual_inheritance(complete_obj, src_obj_offset, src_type_dsc, dst_type_dsc, image_base);
          }
  
          if (base_class)
          {
            dst_obj = pointer_cast(complete_obj.ptr, base_class->cast_info);
          }
        }
      }
      __except (exception_access_violation_seh_filter(GetExceptionCode()))
      {
        // do avoid the nested exceptions for not to come to stack exhaustion in ke-mode
        dst_obj = 0;
      }
    }
    if (is_ref && ! dst_obj)
    {
      throw bad_cast_t();
    }
    return const_cast<pobj_t>(dst_obj);
  }


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
  void* typeid_impl(pcobj_t src_obj)
  {
    if (src_obj)
    {
      __try
      {
        complete_object const complete_obj(src_obj);
        if (complete_obj.check())
        {
          ::size_t image_base = get_image_base(complete_obj.locator);
          complete_object_type_descriptor type_dsc(complete_obj.locator, image_base);
          if ( type_dsc.valid() )
          {
            return const_cast<msvc_internal_data::rtti::type_descriptor*>(*type_dsc);
          }
        }
      }
      __except (exception_access_violation_seh_filter(GetExceptionCode()))
      {
      // do avoid the nested exceptions for not to come to stack exhaustion in ke-mode
      }
    }
    throw bad_typeid_t();
  }


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
  void* cast_to_void_impl(pcobj_t src_obj)
  {
    pcobj_t dst_obj = 0;
    bool throw_bad_cast = false;
    if (src_obj)
    {
      __try
      {
        complete_object const complete_obj(src_obj);
        if (complete_obj.check())
        {
          dst_obj = complete_obj.ptr;
        }
      }
      __except (exception_access_violation_seh_filter(GetExceptionCode()))
      {
      // do avoid the nested exceptions for not to come to stack exhaustion in ke-mode
        throw_bad_cast = true;
      }
    }
    if ( throw_bad_cast )
    {
      throw bad_cast_t();
    }

    return const_cast<pobj_t>(dst_obj);
  }


}  //  namespace rtti
}  //  namespace cpprtl


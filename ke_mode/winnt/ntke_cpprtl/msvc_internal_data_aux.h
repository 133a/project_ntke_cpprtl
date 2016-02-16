/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef MSVC_INTERNAL_DATA_AUX_H_
#define MSVC_INTERNAL_DATA_AUX_H_


#include "msvc_internal_data_iterator.h"


namespace cpprtl
{

  namespace msvc_internal_data_aux
  {
    //////////////////////////////////////////////////////////
    //////////  MSVC EH & RTTI specific types information
    //////////////////////////////////////////////////////////

    typedef cpprtl::msvc_internal_data::array_length_t array_length_t;  // it's to be taken from library's config [eh|rtti]_config.h


    namespace aux_
    {

      ////////////////////////////////////////////////////////
      ////////  convert src_ptr --> dst_ptr
      ////////////////////////////////////////////////////////
 
#ifndef CFG_RVA
      template <typename CL_INFO>
      typename CL_INFO::ADDR_CAST_DST cl_data_cast(typename CL_INFO::ADDR_CAST_SRC const& src, ::size_t const&)
      {
        typename CL_INFO::SAVED_ADDR ptr = 0;
        if (src)
        {
          ptr = src->*CL_INFO::addr_offset;
        }
        return ptr;
      }
#else
      template <typename CL_INFO>
      typename CL_INFO::ADDR_CAST_DST cl_data_cast(typename CL_INFO::ADDR_CAST_SRC const& src, ::size_t const& image_base)
      {
        typename CL_INFO::ADDR_CAST_DST ptr = 0;
        if (src)
        {
          if (typename CL_INFO::SAVED_ADDR rva = src->*CL_INFO::addr_offset)
          {
            ptr = reinterpret_cast<typename CL_INFO::ADDR_CAST_DST>(image_base + rva);
          }
        }
        return ptr;
      }
#endif // CFG_RVA
    }


    template <typename D, typename S>
    struct cl_info_base_with_single_value_iterator
    {
      typedef  cl_info_base_with_single_value_iterator  SELF;
      typedef  D                              DST;
      typedef  S                              SRC;
      typedef  DST const*                     ADDR_CAST_DST;
      typedef  SRC const*                     ADDR_CAST_SRC;
    #ifndef CFG_RVA
      typedef  DST*                           SAVED_ADDR; 
    #else
      typedef  rva_t                          SAVED_ADDR;
    #endif // CFG_RVA
      typedef  SAVED_ADDR S::* const          ADDR_OFFSET;
      typedef  aux_::cl_single_value_iterator<aux_::iterator_traits<DST, ADDR_CAST_DST> >  iterator;

      static ADDR_OFFSET addr_offset;

      static inline ADDR_CAST_DST cl_data_cast(ADDR_CAST_SRC const& src, ::size_t const& image_base)
      {
        return aux_::cl_data_cast<SELF>(src, image_base);
      }
    };


    template <typename D, typename S>
    struct cl_info_base_with_single_value_iterator_and_raw_dst
    {
      typedef  cl_info_base_with_single_value_iterator_and_raw_dst  SELF;
      typedef  D                              DST;
      typedef  S                              SRC;
      typedef  DST                            ADDR_CAST_DST;
      typedef  SRC const*                     ADDR_CAST_SRC;
    #ifndef CFG_RVA
      typedef  DST                            SAVED_ADDR; 
    #else
      typedef  rva_t                          SAVED_ADDR;
    #endif // CFG_RVA
      typedef  SAVED_ADDR S::* const          ADDR_OFFSET;
      typedef  aux_::cl_single_value_iterator<aux_::iterator_traits<DST, ADDR_CAST_DST> >  iterator;

      static ADDR_OFFSET addr_offset;

      static inline ADDR_CAST_DST cl_data_cast(ADDR_CAST_SRC const& src, ::size_t const& image_base)
      {
        return aux_::cl_data_cast<SELF>(src, image_base);
      }
    };


    template <typename D, typename AS, typename SS = AS>
    struct cl_info_base_with_iterator
    {
      typedef  cl_info_base_with_iterator   SELF;
      typedef  D                            DST;
      typedef  AS                           SRC;
      typedef  DST const*                   ADDR_CAST_DST;
      typedef  SRC const*                   ADDR_CAST_SRC;
    #ifndef CFG_RVA
      typedef  DST*                         SAVED_ADDR; 
    #else
      typedef  rva_t                        SAVED_ADDR;
    #endif // CFG_RVA
      typedef  SAVED_ADDR AS::* const       ADDR_OFFSET;
      typedef  array_length_t SS::* const   ARRAY_SIZE_OFFSET;
      typedef  aux_::cl_value_array_iterator<aux_::iterator_traits<DST, ADDR_CAST_DST>, aux_::move_index_forward> fwd_iterator;
      typedef  aux_::cl_value_array_iterator<aux_::iterator_traits<DST, ADDR_CAST_DST>, aux_::move_index_reverse> rev_iterator;
      typedef  fwd_iterator                 iterator;

      static ADDR_OFFSET         addr_offset;
      static ARRAY_SIZE_OFFSET   array_size_offset;

      static inline ADDR_CAST_DST cl_data_cast(ADDR_CAST_SRC const& src, ::size_t const& image_base)
      {
        return aux_::cl_data_cast<SELF>(src, image_base);
      }
    };


    template <typename D, typename AS, typename SS = AS>
    struct cl_info_base_with_array_iterator
    {
      typedef  cl_info_base_with_array_iterator  SELF;
      typedef  D                            DST;
      typedef  AS                           SRC;
      typedef  DST const*                   ADDR_CAST_DST;
      typedef  SRC const*                   ADDR_CAST_SRC;

    #ifndef CFG_RVA
      typedef  DST*                         ARRAY_ITEM;
    #else
      typedef  rva_t                        ARRAY_ITEM;
    #endif // CFG_RVA

      typedef  ARRAY_ITEM                   SAVED_VAL [1] ; 
      typedef  ARRAY_ITEM const*            SAVED_ADDR;
      typedef  SAVED_VAL AS::* const        ARRAY_OFFSET;
      typedef  array_length_t SS::* const   ARRAY_SIZE_OFFSET;

      typedef  aux_::iterator_traits<DST, SAVED_ADDR, ADDR_CAST_DST>            ITER_TR;
      typedef  aux_::cl_ptr_array_iterator<ITER_TR, aux_::move_index_forward>   fwd_iterator;
      typedef  aux_::cl_ptr_array_iterator<ITER_TR, aux_::move_index_reverse>   rev_iterator;
      typedef  fwd_iterator                 iterator;

      static ARRAY_OFFSET        array_offset;
      static ARRAY_SIZE_OFFSET   array_size_offset;

      static inline SAVED_ADDR cl_data_cast(ADDR_CAST_SRC const& src, ::size_t const& image_base)
      {
        return &(src->*array_offset)[0];
      }

    };

  } // namespace msvc_internal_data_aux

} // namespace cpprtl


#endif // include guard


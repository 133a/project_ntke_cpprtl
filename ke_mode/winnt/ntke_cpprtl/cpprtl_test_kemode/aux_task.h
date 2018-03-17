/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2017 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef AUX_TASK_H_
#define AUX_TASK_H_


#include <cstddef>
#include "aux_scoped_ptr.h"


namespace aux_
{

  template <typename TASK>
  class task_bunch
  {
  protected:
    typedef TASK task_type;

    scoped_array_ptr<task_type>  task;
    std::size_t                  task_num;
    std::size_t                  spawned;
    int                          status;

    task_bunch()
      : task      ( 0 )
      , task_num  ( 0 )
      , spawned   ( 0 )
      , status    ( -1 )
    {}

    ~task_bunch()
    {}

  public:
    task_type const& operator[](std::size_t const& idx) const
    {
      return task[idx];
    }
  };


}  // namespace aux_


#endif  // include guard


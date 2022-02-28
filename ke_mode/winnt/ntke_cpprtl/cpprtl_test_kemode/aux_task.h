//============================================
// copyright (c) 2012-2022 project_ntke_cpprtl
// license: the MIT license
//--------------------------------------------


#ifndef AUX_TASK_H_
#define AUX_TASK_H_


#include <cstddef>
#include <new>
#include "aux_scoped_ptr.h"


namespace aux_
{

  template <typename TASK>
  class task_bunch
  {
    typedef TASK task_type;

    scoped_array_ptr<task_type>  tasks;
    std::size_t                  spawned;

  public:
    task_bunch()
      : tasks   ( 0 )
      , spawned ( 0 )
    {}

    ~task_bunch()
    {
      acquire();
    }

    template <typename PAYLOAD>
    bool spawn(std::size_t const task_num, PAYLOAD const& payload)
    {
      tasks.reset(new(std::nothrow) task_type[task_num]);
      if ( !tasks.get() )
      {
        return false;
      }
      for ( spawned = 0 ; spawned < task_num ; ++spawned )
      {
        if ( !tasks[spawned].spawn(payload, task_num > 1 ? spawned : -1) )
        {
          return false;
        }
      }
      return true;
    }

    void acquire()
    {
      while ( spawned > 0 )
      {
        tasks[--spawned].acquire();
      }
    }

    task_type const& operator[](std::size_t const idx) const
    {
      return tasks[idx];
    }
  };


}  // namespace aux_


#endif  // include guard


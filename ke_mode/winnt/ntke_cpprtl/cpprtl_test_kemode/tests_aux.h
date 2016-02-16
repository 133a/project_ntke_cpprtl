/////////////////////////////////////////////////////////////////////////////
////    copyright (c) 2012-2016 project_ntke_cpprtl
////    mailto:kt133a@seznam.cz
////    license: the MIT license
/////////////////////////////////////////////////////////////////////////////


#ifndef TESTS_AUX_H_
#define TESTS_AUX_H_


namespace cpprtl_tests
{

  enum
  {
    RET_SUCCESS = 0

  , RET_ERROR_UNEXPECTED              = -1001
  , RET_ERROR_BAD_ALLOC               = -1002

  , RET_ERROR_WORK_ITEM_SPAWN         = -1101
  , RET_ERROR_WORK_ITEM_ACQUIRE       = -1102
  , RET_ERROR_WORK_ITEM_TASK_SPAWN    = -1103
  , RET_ERROR_WORK_ITEM_TASK_ACQUIRE  = -1104

  , RET_ERROR_DPC_SPAWN               = -1201
  , RET_ERROR_DPC_ACQUIRE             = -1202
  , RET_ERROR_DPC_STATUS_ALLOC        = -1203
  , RET_ERROR_DPC_TASK_ALLOC          = -1204
  , RET_ERROR_DPC_TASK_SPAWN          = -1205
  , RET_ERROR_DPC_TASK_ACQUIRE        = -1206
                                   
  , RET_ERROR_APC_SPAWN_SPECIAL       = -1301
  , RET_ERROR_APC_ACQUIRE_SPECIAL     = -1302
  , RET_ERROR_APC_SPAWN_REGULAR       = -1303
  , RET_ERROR_APC_ACQUIRE_REGULAR     = -1304
  , RET_ERROR_APC_PENDING             = -1305
  , RET_ERROR_APC_ALLOC               = -1306
  , RET_ERROR_APC_TASK_SPAWN          = -1307

  , RET_ERROR_THREAD_NOT_STARTED      = -1401
  , RET_ERROR_THREAD_SPAWN            = -1402
  };


  class test_error
  {
    int status;
  public:
    explicit test_error(int const& st)
      : status ( st )
    {}
    int get() const
    {
      return status;
    }
  };


  typedef void (*testFT)(int&);  // tests interface


  class testFT_adapter  // test thread adapter
  {
    int&    res;
    testFT  ftest;

  public:
    testFT_adapter(testFT f, int& i)
      : res   ( i )
      , ftest ( f )
    {}

    void operator()() const
    {
      if ( ftest )
      {
        ftest(res);
      }
    }
  };

}  //  namespace cpprtl_tests


#endif // include guard


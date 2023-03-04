/*! \file elr_mpl.h.
 *  \brief the interface define header file of the memory pool.
 *  \author 唐锐
 *  \date   2013-06-14
 *
 *  memory of the memory pool is managed with node and slice. node is big 
 *  memory block, slice is small memory block from node. each memory block
 *  allocated from this memory pool is belong to a slice. 
 *  
 *  all slices of a memory pool is of the same size. so this memory pool is
 *  more like a object pool, as all memory block alloced from a memory pool
 *  has the same size. but we can implement a more flexible memory pool by
 *  use many memory pool of different slice size.
 *
 *  node is linked as list, available slice also linked as list. when alloc
 *  memory form the memory pool. first check whether a free slice is exist. 
 *  if exist take one. if not, check whether the newly allocated node has a
 *  never used slice. if has, take a slice form the node. if does not, move 
 *  it to the head of node list and alloc a node and take a slice from it. 
 *  when free a memory block, just move it to the head of available slice
 *  list.
 *
 *  this memory pool is organized in tree structure. when create a memory 
 *  pool, we can specify its parent memory pool by use a pointer to the
 *  parent pool as the first parameter for elr_mpl_create function. when a
 *  memory pool is destroyed it`s child memory pool also be destroyed, so
 *  when we do not need destroy the memory pool and it`s child memory pool 
 *  just destory the parent memory pool. if we do not specify the parent 
 *  pool any more. if we do not specify the parent memory pool when create a
 *  memory pool, the memory pool`s parent memory pool is the global memory 
 *  pool, from which all memory pool`s memory is a allocated. this means we 
 *  can destroy all memory pool by invoke elr_mpl_finalize function.
 *  
 *  this memory pool also support muti-threading using. if we want it works
 *  in muti-theading entironment, we must implement all the six interface
 *  defined in file elr_mtx.h and define ELR_USE_THREAD. fortunately, it is
 *  a very easy job, and the implementation of windows platform is already
 *  provided.
 *
 *  when i make the windows implementation, i take linux into consideration.
 *  so the atomic counter(interger) type and counter value type is defined
 *  separately. under windows there are not a atomic type, just a LONG
 *  illuminated by volatile. we can make assignment between LONG and volatile
 *  LONG. but under linux atomiccounter(interger) type is defined as follow.
 *  typedef struct  {  volatile int counter;  }  atomic_t; .
 *  assignment between int and atomic_t is against the c language syntax.
 *
 *  this memory pool had been used in manay production environment and its
 *  ability had been proved. there is still plenty of space for improvement.
 *  now each memory pool has its own mutex, under many circumstance this is
 *  not a must. so thera are at least two point need to be improved.
 *  first, reduce the mutex consume. second, make it to be a all-purpose
 *  memory pool, like appache memory pool, many regular size of memory can be
 *  be allocated from it.
 */

#ifndef __ELR_MPL_H__
#define __ELR_MPL_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
  #define ELR_HELPER_LIB_IMPORT __declspec(dllimport)
  #define ELR_HELPER_LIB_EXPORT __declspec(dllexport)
  #define ELR_HELPER_LIB_LOCAL
#else
  #if __GNUC__ >= 4
    #define ELR_HELPER_LIB_IMPORT __attribute__ ((visibility ("default")))
    #define ELR_HELPER_LIB_EXPORT __attribute__ ((visibility ("default")))
    #define ELR_HELPER_LIB_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define ELR_HELPER_LIB_IMPORT __attribute__((dllimport))
    #define ELR_HELPER_LIB_EXPORT __attribute__((dllexport))
    #define ELR_HELPER_LIB_LOCAL
  #endif
#endif

#ifdef ELR_MPL_EXPORTS
  #define ELR_MPL_API ELR_HELPER_LIB_EXPORT
#elif ELR_MPL_IMPORTS
  #define ELR_MPL_API ELR_HELPER_LIB_IMPORT
#else
  #define ELR_MPL_API
#endif

#define ELR_MPL_LOCAL ELR_HELPER_LIB_LOCAL

/*! \brief memory pool type.
 *
 *  it is highly recommend that you declare a elr_mpl_t variable 
 *  with the following initializing statement.
 *  elr_mpl_t mpl = ELR_MPL_INITIALIZER;
 *  an elr_mpl_t variable is only valid when 
 *  it was initialized directly or indirectly by elr_mpl_create`s return value.
 *  don`t modify it`s members manualy.
 */
typedef struct __elr_mpl_t
{
	void*  pool; /*!< the actual handler of internal memory pool object. */
	int    tag; /*!< the identity code of memory pool object. */
}
elr_mpl_t,*elr_mpl_ht;


/*! \def ELR_MPL_INITIALIZER
 *  \brief elr_mpl_t constant for initializing.
 */
#define ELR_MPL_INITIALIZER   {NULL,0}

/*
** 初始化内存池，内部创建一个全局内存池。
** 该方法可以被重复调用，如果内存池模块已经初始化，该方法直接返回。
** 全局内存池中不容纳内存块，用于容纳所有创建的内存池。
** 返回0表示初始化失败
*/
/*! \brief initialize memory pool module.
 *  \retval zero if failed.
 *  
 *  this function can invoke many times in one process.
 *  bear in mind that one should invoke elr_mpl_finalize 
 *  the same times as invoke this function in one process
 *  when the process about to end.
 */
ELR_MPL_API int elr_mpl_init();

/*
** 创建一个内存池，并指定分配单元大小。
** 第一个参数表示父内存池，如果其为NULL，表示创建的内存池的父内存池是全局内存池。
*/
/*! \brief create a memory pool.
 *  \param fpool the parent pool of the about to created pool.
 *  \param obj_size the size of memory block can alloc from the pool.
 *  \retval NULL if failed.
 *
 *  in fact this memory pool is more like object pool.
 */
ELR_MPL_API elr_mpl_t elr_mpl_create(elr_mpl_ht fpool,size_t obj_size);

/*
** 判断内存池是否是有效的，一般在创建完成后立即调用。
** 返回0表示无效
** pool不可为NULL
*/
/*! \brief verifies that a memory pool is valid or not.
 *  \param pool  pointer to a elr_mpl_t type variable.
 *  \retval zero if invalid.
 */
ELR_MPL_API int  elr_mpl_avail(elr_mpl_ht pool);

/*
** 从内存池中申请内存，其大小为内存池的分配单元大小。
** pool不可为NULL
*/
/*! \brief alloc a memory block from a memory pool.
 *  \param pool  pointer to a elr_mpl_t type variable.
 *  \retval NULL if failed.
 *
 *  size of the memory block alloced is the second parameter
 *  of elr_mpl_create when create the pool.
 */
ELR_MPL_API void* elr_mpl_alloc(elr_mpl_ht pool);

/*
** 获取从内存池中申请的内存块的尺寸。
*/
/*! \brief get the size of a memory block from a memory pool.
 *  \param mem pointer to a memory block from a memory pool.
 *  \retval size of the memory block.
 */
ELR_MPL_API size_t elr_mpl_size(void* mem);

/*
** 将内存退回给内存池。
*/
/*! \brief give back a memory block to it`s from memory pool.
 */
ELR_MPL_API void elr_mpl_free(void* mem);

/*
** 销毁内存池和其子内存池。
*/
/*! \brief destroy a memory pool and it`s child pools.
 */
ELR_MPL_API void elr_mpl_destroy(elr_mpl_ht pool);

/*
** 终止内存池模块，会销毁全局内存池及其子内存池。
** 程序中创建的其它内存池如果没有显示的释放，执行此操作后也会被释放。
*/
/*! \brief finalize memory pool module.
 * 
 *  when finalize is finished all memory pools will be destroyed.
 *  make sure that when finalize is in process all memory pool is not in using.
 *  so it is recommend that elr_mpl_finalize invoked only in the end of a process.
 */
ELR_MPL_API void elr_mpl_finalize();

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif

#ifndef __ELR_MTX_H__
#define __ELR_MTX_H__

#ifdef   __WIN32__
#include <windows.h>

/*! \brief platform independent mutex type.
 *
 *  it is highly recommend that you declare a elr_mpl_t variable 
 *  with the following initializing statement
 *  elr_mpl_t mpl = ELR_MPL_INITIALIZER;
 *  an elr_mpl_t variable is only valid when 
 *  it was initialized directly or indirectly by elr_mpl_create`s return value.
 */
typedef struct __elr_mtx
{
	CRITICAL_SECTION  _cs;/*!< the windows critical section object. */
}
elr_mtx;

/** platform independent atomic counter type. */
typedef volatile LONG         elr_atomic_t;

/** platform independent counter integer type. */
typedef LONG                  elr_counter_t;   
 
/** platform independent zero initial value of atomic counter type. */
#define   ELR_ATOMIC_ZERO     0



/*
** 原子自增操作
*/
/*! \brief atomic increment operation.
 *  \param v pointer to a atomic counter type variable.
 *  \retval the integer value of v after increment.
 *
 *  make a increment of v by one atomicly.
 */
elr_counter_t elr_atomic_inc(elr_atomic_t* v);

/*
** 原子自减操作
*/
/*! \brief atomic decrement operation.
 *  \param v pointer to a atomic counter type variable.
 *  \retval the integer value of v after decrement.
 *
 *  make a decrement of v by one atomicly.
 */
elr_counter_t elr_atomic_dec(elr_atomic_t* v);

/*
** 初始化互斥体，返回0表示初始化失败
** 如果初始化失败就不需要再调用elr_mtx_finalize
*/
/*! \brief initialize a mutex.
 *  \param mtx pointer to a mutex.
 *  \retval zero if failed.
 */
int  elr_mtx_init(elr_mtx *mtx);

/*! \brief waits on a mutex.
 *  \param mtx pointer to a mutex.
 */
void elr_mtx_lock (elr_mtx *mtx);

/*! \brief releases on a mutex.
 *  \param mtx pointer to a mutex.
 */
void elr_mtx_unlock(elr_mtx *mtx);

/*! \brief initialize a mutex.
 *  \param mtx pointer to a mutex.
 */
void elr_mtx_finalize(elr_mtx *mtx);

#endif  // win32 

#endif // elr_mpl 

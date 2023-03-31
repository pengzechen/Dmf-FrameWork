#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dmfserver/elr_mpl/elr_mpl.h"

#ifdef ELR_USE_THREAD
#include "elr_mpl/elr_mtx.h"
#endif // ELR_USE_THREAD

/*最大内存切片的尺寸*/
/** max memory node size. it can be changed to fit the memory consume more.  */
#define ELR_MAX_SLICE_SIZE       32768  /*32KB*/
/*最大内存切片的数目*/
/** max memory slice size. it can be changed to fit the memory consume more. */
#define ELR_MAX_SLICE_COUNT      64  /*64*/

#define ELR_ALIGN(size, boundary)     (((size) + ((boundary) - 1)) & ~((boundary) - 1)) 

/*! \brief memory node type.
 *
 */
typedef struct __elr_mem_node
{
    struct __elr_mem_pool       *owner;
    struct __elr_mem_node       *prev;
    struct __elr_mem_node       *next;
	/*空闲的内存切片链表头*/
    struct __elr_mem_slice      *free_slice_head;
	/*空闲的内存切片链表尾*/
    struct __elr_mem_slice      *free_slice_tail;
	/*正在使用的slice的数量*/
    size_t                       using_slice_count;
	/*使用过的slice的数量*/
    size_t                       used_slice_count;
    char                        *first_avail;
}
elr_mem_node;

typedef struct __elr_mem_slice
{
    struct __elr_mem_slice      *prev;
    struct __elr_mem_slice      *next;
	/*内存切片所属的内存节点*/
    elr_mem_node                *node;
	/*内切片的标签，初始值为0，每一次从内存池中取出和归还都会加1*/
	int                          tag;
}
elr_mem_slice;

typedef struct __elr_mem_pool
{
    struct __elr_mem_pool       *parent;
    struct __elr_mem_pool       *first_child;
    struct __elr_mem_pool       *prev;
    struct __elr_mem_pool       *next;
    size_t                       slice_count;
    size_t                       slice_size;
	size_t                       object_size;
    size_t                       node_size;
	/*所有elr_mem_node组成的链表*/
    elr_mem_node                *first_node;
	/*刚刚创建的elr_mem_node*/
    elr_mem_node                *newly_alloc_node;
	/*空闲的内存切片链表*/
    elr_mem_slice               *first_free_slice;
	/*容纳本内存池对象的内存切片的标签*/
	int                          slice_tag;
#ifdef ELR_USE_THREAD
    elr_mtx                                pool_mutex;
#endif // ELR_USE_THREAD
}
elr_mem_pool;


/*全局内存池*/
static elr_mem_pool   g_mem_pool;

/*全局内存池引用计数*/
#ifdef ELR_USE_THREAD
static elr_atomic_t     g_mpl_refs = ELR_ATOMIC_ZERO;
#else
static long           g_mpl_refs = 0;
#endif // ELR_USE_THREAD


/*为内存池申请一个内存节点*/
void                      _elr_alloc_mem_node(elr_mem_pool *pool);
/*移除一个未使用的NODE，返回0表示没有移除*/
int                       _elr_remove_unused_node(elr_mem_node* node);
/*在内存池的刚刚创建的内存节点中分配一个内存切片*/
elr_mem_slice*            _elr_slice_from_node(elr_mem_pool *pool);
/*在内存池中分配一个内存切片，该方法将会调用上述两方法*/
elr_mem_slice*            _elr_slice_from_pool(elr_mem_pool *pool);
/*销毁内存池，inter表示是否是内部调用*/
void                      _elr_inter_mpl_destory(elr_mem_pool *pool, int inter);

/*
** 初始化内存池，内部创建一个全局内存池。
** 该方法可以被重复调用。
** 如果内存池模块已经初始化，仅仅增加引用计数然后返回。
*/
ELR_MPL_API int elr_mpl_init()
{
#ifdef ELR_USE_THREAD
	elr_counter_t refs = elr_atomic_inc(&g_mpl_refs);
	if(refs == 1)
	{
#else
	g_mpl_refs++;
	if(g_mpl_refs == 1)
	{
#endif // ELR_USE_THREAD
		g_mem_pool.parent = NULL;
		g_mem_pool.first_child = NULL;
		g_mem_pool.prev = NULL;
		g_mem_pool.next = NULL;
		g_mem_pool.object_size = sizeof(elr_mem_pool);
		g_mem_pool.slice_size = ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int))
			+ ELR_ALIGN(sizeof(elr_mem_pool),sizeof(int));
		g_mem_pool.slice_count = ELR_MAX_SLICE_COUNT;
		g_mem_pool.node_size = g_mem_pool.slice_size*g_mem_pool.slice_count 
			+ ELR_ALIGN(sizeof(elr_mem_node),sizeof(int));
		g_mem_pool.first_node = NULL;
		g_mem_pool.newly_alloc_node = NULL;
		g_mem_pool.first_free_slice = NULL;
		g_mem_pool.slice_tag = 0;
#ifdef ELR_USE_THREAD
		if(elr_mtx_init(&g_mem_pool.pool_mutex) == 0)
		{
			elr_atomic_dec(&g_mpl_refs);
			return 0;
		}

#endif // ELR_USE_THREAD
	}

	return 1;
}

/*
** 创建一个内存池，并指定最大分配单元大小。
** 第一个参数表示父内存池，如果其为NULL，表示创建的内存池的父内存池是全局内存池。
*/
ELR_MPL_API elr_mpl_t elr_mpl_create(elr_mpl_ht fpool,size_t obj_size)
{
	elr_mpl_t      mpl = ELR_MPL_INITIALIZER;
	elr_mem_slice *pslice = NULL;
    elr_mem_pool  *pool = NULL;

	assert(fpool==NULL || elr_mpl_avail(fpool)!=0);

	if((pslice = _elr_slice_from_pool(&g_mem_pool)) == NULL)
		return mpl;
	pool = (elr_mem_pool*)((char*)pslice
		+ ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));

	pool->slice_tag = pslice->tag;
    pool->first_child = NULL;
    pool->parent = fpool==NULL?&g_mem_pool:(elr_mem_pool*)fpool->pool;

#ifdef ELR_USE_THREAD	
	if(pool->slice_tag == 0)
	{
		if(elr_mtx_init(&pool->pool_mutex)==0)
		{
			elr_mpl_free(pool);
			return mpl;
		}
	}

	elr_mtx_lock(&pool->pool_mutex);
#endif // ELR_USE_THREAD
	pool->object_size = obj_size;
    pool->slice_size = ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int))
		+ ELR_ALIGN(obj_size,sizeof(int));
    if(pool->slice_size < ELR_MAX_SLICE_SIZE)
        pool->slice_count = ELR_MAX_SLICE_COUNT 
		- pool->slice_size*(ELR_MAX_SLICE_COUNT-1)/ELR_MAX_SLICE_SIZE;
    else
        pool->slice_count = 1;
    pool->node_size = pool->slice_size*pool->slice_count 
		+ ELR_ALIGN(sizeof(elr_mem_node),sizeof(int));
    pool->first_node = NULL;
    pool->newly_alloc_node = NULL;
    pool->first_free_slice = NULL;
#ifdef ELR_USE_THREAD
	elr_mtx_unlock(&pool->pool_mutex);
#endif // ELR_USE_THREAD

#ifdef ELR_USE_THREAD
    elr_mtx_lock(&pool->parent->pool_mutex);
#endif // ELR_USE_THREAD
    pool->prev = NULL;
    pool->next = pool->parent->first_child;
    if(pool->next != NULL)
        pool->next->prev = pool;
	pool->parent->first_child = pool;
#ifdef ELR_USE_THREAD
	elr_mtx_unlock(&pool->parent->pool_mutex);
#endif // ELR_USE_THREAD

	mpl.pool = pool;
	mpl.tag = pool->slice_tag;

    return mpl;
}

/*
** 判断内存池是否是有效的，一般在创建完成后立即调用。
** 返回0表示无效
** pool不可为NULL
*/
ELR_MPL_API int  elr_mpl_avail(elr_mpl_ht hpool)
{
	int              ret = 1;
	elr_mem_slice   *pslice = NULL;

	if(hpool->pool == NULL)
	{
		ret = 0;
	}
	else
	{
		pslice = (elr_mem_slice*)((char*)(hpool->pool) 
			- ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));
		if (hpool->tag != pslice->tag)
			ret = 0;
	}

	return ret;
}

/*
** 从内存池中申请内存。
*/
ELR_MPL_API void*  elr_mpl_alloc(elr_mpl_ht hpool)
{
	elr_mem_slice *pslice = NULL;
	assert(hpool != NULL  && elr_mpl_avail(hpool)!=0);
	pslice = _elr_slice_from_pool((elr_mem_pool*)hpool->pool);

    if(pslice == NULL)
        return NULL;
    else
        return (char*)pslice
		+ ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int));
}


/*
** 获取从内存池中申请的内存块的尺寸。
*/
ELR_MPL_API size_t elr_mpl_size(void* mem)
{
    elr_mem_slice *pslice = (elr_mem_slice*)((char*)mem
		- ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));
    return pslice->node->owner->object_size;
}

/*
** 将内存退回给内存池。执行该方法也可能将内存退回给系统。
*/
ELR_MPL_API void  elr_mpl_free(void* mem)
{
    elr_mem_slice *pslice = (elr_mem_slice*)((char*)mem 
		- ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));
    elr_mem_node* pnode = pslice->node;
    elr_mem_pool* pool = pnode->owner;

#ifdef ELR_USE_THREAD
    elr_mtx_lock(&pool->pool_mutex);
#endif // ELR_USE_THREAD
	pslice->tag++;
	pnode->using_slice_count--;
	if(pnode->free_slice_head == NULL)
	{
		pnode->free_slice_head = pslice;
		pnode->free_slice_tail = pslice;
		pslice->next = pool->first_free_slice;
		if(pool->first_free_slice != NULL)
			pool->first_free_slice->prev = pslice;
		pool->first_free_slice = pslice;
	}
	else
	{
		if(!_elr_remove_unused_node(pnode))
		{
			pslice->next = pnode->free_slice_tail->next;
			if(pnode->free_slice_tail->next != NULL)
				pnode->free_slice_tail->next->prev = pslice;
			pnode->free_slice_tail->next = pslice;
			pslice->prev = pnode->free_slice_tail;
			pnode->free_slice_tail = pslice;
		}
	}
#ifdef ELR_USE_THREAD
    elr_mtx_unlock(&pool->pool_mutex);
#endif // ELR_USE_THREAD
    return;
}

/*
** 销毁内存池和其子内存池。
*/
ELR_MPL_API void elr_mpl_destroy(elr_mpl_ht hpool)
{
    elr_mem_pool  *pool = NULL;
	elr_mem_slice *pslice = NULL;
	int            flag = 0;

	assert(hpool!=NULL && elr_mpl_avail(hpool)!=0);
	pool = (elr_mem_pool*)hpool->pool;
	assert(pool->parent != NULL);

#ifdef ELR_USE_THREAD
	elr_mtx_lock(&pool->parent->pool_mutex);
    elr_mtx_lock(&pool->pool_mutex);
#endif // ELR_USE_THREAD

	pslice = (elr_mem_slice*)((char*)pool 
		- ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));

	if(hpool->tag == pool->slice_tag)
		flag = 1;

	if(flag == 1)
	{
		if(pool->next != NULL)
			pool->next->prev = pool->prev;

		if(pool->prev == NULL)
			pool->parent->first_child = pool->next;
		else
			pool->prev->next = pool->next;
	}

#ifdef ELR_USE_THREAD
	elr_mtx_unlock(&pool->parent->pool_mutex);
#endif // ELR_USE_THREAD
	
	if(flag == 1)
		_elr_inter_mpl_destory(pool,0);

	hpool->pool = NULL;
	hpool->tag = 0;
#ifdef ELR_USE_THREAD
    elr_mtx_unlock(&pool->pool_mutex);
#endif // ELR_USE_THREAD    
}

/*
** 终止内存池模块，会销毁全局内存池及其子内存池。
** 程序中创建的其它内存池如果没有显示的释放，执行此操作后也会被释放。
*/
ELR_MPL_API void elr_mpl_finalize()
{
#ifdef ELR_USE_THREAD
	elr_counter_t   refs = 1;
    elr_mtx_lock(&g_mem_pool.pool_mutex);
	refs = elr_atomic_dec(&g_mpl_refs);
    if(refs == 0)
    {
#else
	g_mpl_refs--;
	if(g_mpl_refs == 0)
	{
#endif // ELR_USE_THREAD
		_elr_inter_mpl_destory(&g_mem_pool,0);
    }

#ifdef ELR_USE_THREAD
    elr_mtx_unlock(&g_mem_pool.pool_mutex);
#endif // ELR_USE_THREAD
}


void _elr_alloc_mem_node(elr_mem_pool *pool)
{
    elr_mem_node* pnode = (elr_mem_node*)malloc(pool->node_size);
    if(pnode == NULL)
        return;
    pool->newly_alloc_node = pnode;
    pnode->owner = pool;
    pnode->first_avail = (char*)pnode
		+ ELR_ALIGN(sizeof(elr_mem_node),sizeof(int));

	pnode->free_slice_head = NULL;
    pnode->free_slice_tail = NULL;
    pnode->used_slice_count = 0;
	pnode->using_slice_count = 0;
	pnode->prev = NULL;

    if(pool->first_node == NULL)
    {
        pool->first_node = pnode;
        pnode->next = NULL;		
    }
    else
    {
        pnode->next = pool->first_node;
        pool->first_node->prev = pnode;
        pool->first_node = pnode;
    }
}

/*移除一个未使用的NODE，返回0表示没有移除*/
int _elr_remove_unused_node(elr_mem_node* pnode)
{
	int  free_node_flag = 0;
	if(pnode->using_slice_count == 0)
	{
		if(pnode->free_slice_tail->next!=NULL)
		{
			pnode->free_slice_tail->next->prev = pnode->free_slice_head->prev;
			free_node_flag = 1;
		}

		if(pnode->free_slice_head->prev!=NULL)
		{
			pnode->free_slice_head->prev->next = pnode->free_slice_tail->next;
			free_node_flag = 1;
		}
		else /*if(pnode->owner->first_free_slice == pnode->free_slice_head)*/
		{
			if(free_node_flag)
				pnode->owner->first_free_slice = pnode->free_slice_tail->next;
		}

		if(free_node_flag)
		{
			if(pnode->prev != NULL)
				pnode->prev->next = pnode->next;
			else
			{
				pnode->owner->first_node = pnode->next;
				pnode->next->prev = NULL;
			}

			if(pnode->next != NULL)
				pnode->next->prev = pnode->prev;

			free(pnode);
		}
	}

	return free_node_flag;
}

elr_mem_slice* _elr_slice_from_node(elr_mem_pool *pool)
{
    elr_mem_slice *pslice = NULL;

    if(pool->newly_alloc_node != NULL)
    {
        pool->newly_alloc_node->used_slice_count++;
		pool->newly_alloc_node->using_slice_count++;
        pslice = (elr_mem_slice*)pool->newly_alloc_node->first_avail;
		memset(pslice,0,pool->slice_size);
		pslice->next = NULL;
		pslice->prev = NULL;
		pslice->tag = 0;
        pool->newly_alloc_node->first_avail += pool->slice_size;
        pslice->node = pool->newly_alloc_node;
        if(pool->newly_alloc_node->used_slice_count == pool->slice_count)
            pool->newly_alloc_node = NULL;
    }

    return pslice;
}

/*
** 从内存池中申请内存。
*/
elr_mem_slice* _elr_slice_from_pool(elr_mem_pool* pool)

{
    elr_mem_slice *pslice = NULL;

	assert(pool != NULL);

#ifdef ELR_USE_THREAD
    elr_mtx_lock(&pool->pool_mutex);
#endif // ELR_USE_THREAD

    if(pool->first_free_slice != NULL)
    {
        pslice = pool->first_free_slice;
		pslice->node->free_slice_head = pslice->next;
        pool->first_free_slice = pslice->next;
		if(pool->first_free_slice != NULL)
			pool->first_free_slice->prev = NULL;
		pslice->next = NULL;
		pslice->prev = NULL;
		pslice->tag++;
		pslice->node->using_slice_count++;
    }
    else
    {
        if(pool->newly_alloc_node == NULL)
            _elr_alloc_mem_node(pool);
        pslice = _elr_slice_from_node(pool);
    }
#ifdef ELR_USE_THREAD
    elr_mtx_unlock(&pool->pool_mutex);
#endif // ELR_USE_THREAD

	return pslice;
}


void _elr_inter_mpl_destory(elr_mem_pool *pool,int inter)
{
    elr_mem_pool   *temp_pool = NULL;
    elr_mem_node  *temp_node = NULL;
	size_t                  index = 0;

#ifdef ELR_USE_THREAD
	if(inter==1)
        elr_mtx_lock(&pool->pool_mutex);
#endif // ELR_USE_THREAD	

	temp_pool = pool->first_child;

    while(temp_pool != NULL)
    {
		pool->first_child = temp_pool->next;
		temp_pool->parent = NULL;
		temp_pool->slice_tag = -1;

		_elr_inter_mpl_destory(temp_pool,1);
		
        temp_pool = pool->first_child;
	}

	temp_node = pool->first_node;
	while(temp_node != NULL)
	{		
		pool->first_node = temp_node->next;

#ifdef ELR_USE_THREAD
		if(pool == &g_mem_pool)
		{
			index = 0;
			temp_pool = (elr_mem_pool*)((char*)temp_node
				+ ELR_ALIGN(sizeof(elr_mem_node),sizeof(int))
				+ ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));
			while(index < temp_node->used_slice_count)
			{
				elr_mtx_finalize(&pool->pool_mutex);
				temp_pool = (elr_mem_pool*)((char*)pool+g_mem_pool.slice_size);
				index++;
			}
		}
#endif // ELR_USE_THREAD

		free(temp_node);
		temp_node = pool->first_node ;
	}

	/*如果不是根节点*/
	if(pool != &g_mem_pool)
		elr_mpl_free(pool);

#ifdef ELR_USE_THREAD
	if(inter==1)
        elr_mtx_unlock(&pool->pool_mutex);
#endif // ELR_USE_THREAD
}

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "elr_mpl/elr_mpl.h"

#ifdef ELR_USE_THREAD
#include "elr_mpl/elr_mtx.h"
#endif // ELR_USE_THREAD

/*����ڴ���Ƭ�ĳߴ�*/
/** max memory node size. it can be changed to fit the memory consume more.  */
#define ELR_MAX_SLICE_SIZE       32768  /*32KB*/
/*����ڴ���Ƭ����Ŀ*/
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
	/*���е��ڴ���Ƭ����ͷ*/
    struct __elr_mem_slice      *free_slice_head;
	/*���е��ڴ���Ƭ����β*/
    struct __elr_mem_slice      *free_slice_tail;
	/*����ʹ�õ�slice������*/
    size_t                       using_slice_count;
	/*ʹ�ù���slice������*/
    size_t                       used_slice_count;
    char                        *first_avail;
}
elr_mem_node;

typedef struct __elr_mem_slice
{
    struct __elr_mem_slice      *prev;
    struct __elr_mem_slice      *next;
	/*�ڴ���Ƭ�������ڴ�ڵ�*/
    elr_mem_node                *node;
	/*����Ƭ�ı�ǩ����ʼֵΪ0��ÿһ�δ��ڴ����ȡ���͹黹�����1*/
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
	/*����elr_mem_node��ɵ�����*/
    elr_mem_node                *first_node;
	/*�ոմ�����elr_mem_node*/
    elr_mem_node                *newly_alloc_node;
	/*���е��ڴ���Ƭ����*/
    elr_mem_slice               *first_free_slice;
	/*���ɱ��ڴ�ض�����ڴ���Ƭ�ı�ǩ*/
	int                          slice_tag;
#ifdef ELR_USE_THREAD
    elr_mtx                                pool_mutex;
#endif // ELR_USE_THREAD
}
elr_mem_pool;


/*ȫ���ڴ��*/
static elr_mem_pool   g_mem_pool;

/*ȫ���ڴ�����ü���*/
#ifdef ELR_USE_THREAD
static elr_atomic_t     g_mpl_refs = ELR_ATOMIC_ZERO;
#else
static long           g_mpl_refs = 0;
#endif // ELR_USE_THREAD


/*Ϊ�ڴ������һ���ڴ�ڵ�*/
void                      _elr_alloc_mem_node(elr_mem_pool *pool);
/*�Ƴ�һ��δʹ�õ�NODE������0��ʾû���Ƴ�*/
int                       _elr_remove_unused_node(elr_mem_node* node);
/*���ڴ�صĸոմ������ڴ�ڵ��з���һ���ڴ���Ƭ*/
elr_mem_slice*            _elr_slice_from_node(elr_mem_pool *pool);
/*���ڴ���з���һ���ڴ���Ƭ���÷��������������������*/
elr_mem_slice*            _elr_slice_from_pool(elr_mem_pool *pool);
/*�����ڴ�أ�inter��ʾ�Ƿ����ڲ�����*/
void                      _elr_inter_mpl_destory(elr_mem_pool *pool, int inter);

/*
** ��ʼ���ڴ�أ��ڲ�����һ��ȫ���ڴ�ء�
** �÷������Ա��ظ����á�
** ����ڴ��ģ���Ѿ���ʼ���������������ü���Ȼ�󷵻ء�
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
** ����һ���ڴ�أ���ָ�������䵥Ԫ��С��
** ��һ��������ʾ���ڴ�أ������ΪNULL����ʾ�������ڴ�صĸ��ڴ����ȫ���ڴ�ء�
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
** �ж��ڴ���Ƿ�����Ч�ģ�һ���ڴ�����ɺ��������á�
** ����0��ʾ��Ч
** pool����ΪNULL
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
** ���ڴ���������ڴ档
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
** ��ȡ���ڴ����������ڴ��ĳߴ硣
*/
ELR_MPL_API size_t elr_mpl_size(void* mem)
{
    elr_mem_slice *pslice = (elr_mem_slice*)((char*)mem
		- ELR_ALIGN(sizeof(elr_mem_slice),sizeof(int)));
    return pslice->node->owner->object_size;
}

/*
** ���ڴ��˻ظ��ڴ�ء�ִ�и÷���Ҳ���ܽ��ڴ��˻ظ�ϵͳ��
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
** �����ڴ�غ������ڴ�ء�
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
** ��ֹ�ڴ��ģ�飬������ȫ���ڴ�ؼ������ڴ�ء�
** �����д����������ڴ�����û����ʾ���ͷţ�ִ�д˲�����Ҳ�ᱻ�ͷš�
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

/*�Ƴ�һ��δʹ�õ�NODE������0��ʾû���Ƴ�*/
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
** ���ڴ���������ڴ档
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

	/*������Ǹ��ڵ�*/
	if(pool != &g_mem_pool)
		elr_mpl_free(pool);

#ifdef ELR_USE_THREAD
	if(inter==1)
        elr_mtx_unlock(&pool->pool_mutex);
#endif // ELR_USE_THREAD
}

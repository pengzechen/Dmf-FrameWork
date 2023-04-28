/* 
    *  Copyright 2023 Ajax
    *
    *  Licensed under the Apache License, Version 2.0 (the "License");
    *  you may not use this file except in compliance with the License.
    *
    *  You may obtain a copy of the License at
    *
    *    http://www.apache.org/licenses/LICENSE-2.0
    *    
    *  Unless required by applicable law or agreed to in writing, software
    *  distributed under the License is distributed on an "AS IS" BASIS,
    *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    *  See the License for the specific language governing permissions and
    *  limitations under the License. 
    *
    */

#ifndef __MPOOL_INCLUDE__
#define __MPOOL_INCLUDE__

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node_t {
    void            *data;
    struct node_t   *next;
    int             used;
} node_t;

typedef struct {
    node_t              *head;
    pthread_mutex_t     lock;
    pthread_cond_t      cond;

    // 当前可用块数
    int                 count;
    int                 block_size;
    int                 total_size;
    // 只申请一次内存  存放在 mem_pool 中
    char                *mem_pool;
} pool_t;


#ifdef __cplusplus
extern "C" {
#endif

    void pool_init(int block_size, int total_size);
    void pool_destroy();
    void *pool_alloc();
    void pool_free(void *data);
    
    void pool_init2(int block_size, int total_size);
    void pool_destroy2();
    void *pool_alloc2();
    void pool_free2(void *data);
#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif // __MPOOL_INCLUDE__
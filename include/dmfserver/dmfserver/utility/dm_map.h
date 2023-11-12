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
#ifndef __DM_MAP_INCLUDE__
#define __DM_MAP_INCLUDE__

#include <dmfserver/utility/dm_hash.h>
#define HASH_FUNCTION BKDRHash


#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 定义哈希映射的节点结构
typedef struct hashmap_node_t {
    char * key;
    void * value;
    struct hashmap_node_t *next;
} hashmap_node_t;

// 定义哈希映射结构
typedef struct hashmap_t {
    size_t size;
    hashmap_node_t ** buckets;
} hashmap_t;

typedef hashmap_t * hashmap_tp;

#ifdef __cplusplus
extern "C" {
#endif

// 初始化哈希映射
hashmap_tp hashmap_create( size_t size );

// 插入键值对到哈希映射
int hashmap_insert(hashmap_tp hashmap, hashmap_node_t * node);

// 获取指定键的值
void * hashmap_get( hashmap_tp hashmap, char * key );

// 删除指定键的节点
int hashmap_remove( hashmap_tp hashmap, char * key );

// 释放哈希映射的内存
void hashmap_destroy( hashmap_tp hashmap );

// hashmap 测试
int test_map();

#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif


#endif  // __DM_MAP_INCLUDE__
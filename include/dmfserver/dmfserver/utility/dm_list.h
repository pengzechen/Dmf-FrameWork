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


#ifndef __DM_LIST_INCLUDE__
#define __DM_LIST_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 定义一个包含数据内容的结构体
typedef struct Data {
    int value;
} Data;

// 定义一个节点结构体
typedef struct Node {
    struct Node *next;
    Data data;
} Node;

// 定义一个列表结构体
typedef struct List {
    Node *head;
    size_t size;
} List;


#ifdef __cplusplus
extern "C" {
#endif
    List *list_create();
    void list_append(List *list, Data data);
    size_t list_size(List *list);
    bool list_find(List *list, Data data);
    void list_destroy(List *list);

#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif


#endif  // __DM_LIST_INCLUDE__
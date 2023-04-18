/* 
Copyright 2023 Ajax

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
*/

#ifndef __DM_QUEUE_INCLUDE__
#define __DM_QUEUE_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

// 定义一个包含数据内容的结构体
typedef struct Data {
    int value;
} Data;

// 定义一个节点结构体
typedef struct Node {
    struct Node *next;
    Data data;
} Node;

// 定义一个队列结构体
typedef struct Queue {
    Node *head;
    Node *tail;
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

#ifdef __cplusplus
extern "C" {
#endif

	Queue *queue_create();
	void queue_enqueue(Queue *queue, Data data);
	Data queue_dequeue(Queue *queue);
	size_t queue_size(Queue *queue);
	void queue_destroy(Queue *queue);

#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif


#endif // __DM_QUEUE_INCLUDE__
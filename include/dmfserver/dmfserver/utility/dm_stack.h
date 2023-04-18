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

#ifndef __DM_STACK_INCLUDE__ 
#define __DM_STACK_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

// 定义一个包含数据内容的结构体
typedef struct Data {
    int value;
} Data;

// 定义一个堆栈结构体
typedef struct Stack {
    Data *data;
    size_t size;
    size_t capacity;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Stack;


#ifdef __cplusplus
extern "C" {
#endif

	Stack *stack_create(size_t capacity);
	void stack_push(Stack *stack, Data data);
	Data stack_pop(Stack *stack);
	Data stack_top(Stack *stack);
	size_t stack_size(Stack *stack);
	void stack_destroy(Stack *stack);
	
	

#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif


#endif // __DM_STACK_INCLUDE__
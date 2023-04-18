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

#ifndef __DM_THREAD_POOL_INCLUDE__
#define __DM_THREAD_POOL_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/time.h>
#include <stdbool.h>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif


typedef struct task_t {
    void            	(*func)(void*);
    void*           	arg;
    struct task_t*  	next;
	
} task_t;

typedef struct thread_t {
    pthread_t 			tid;
    struct 				thread_pool_t *pool;
    struct thread_t* 	next;
    struct timeval 		end_time;
	
} thread_t;

/*
typedef struct timer_t {
    struct timeval 		end_time;
    void 				(*func)(void*);
    void*				arg;
	struct timer_t* 	next;
	
} timer_t;
*/

typedef struct thread_pool_t {
    pthread_mutex_t 	lock;
    pthread_cond_t 		notify;
	
    task_t  *  			task_list;
    thread_t*  			thread_list;
    //timer_t *  			timer_list;
	
    int 				thread_count;
    bool 				shutdown;
	
} thread_pool_t;

#ifdef __cplusplus
extern "C" {
#endif

	void				sleep_microseconds(int microseconds);
	void* 				thread_func(void *arg);
	void 				thread_pool_destroy(thread_pool_t *pool);
	thread_pool_t*		thread_pool_create(int thread_count);
	int 				thread_pool_add_task(thread_pool_t *pool, void (*func)(void*), void *arg);
	//int 				add_timer(thread_pool_t *pool, int seconds, void (func)(void), void *arg);
	int 				is_thread_pool_empty(thread_pool_t *pool);

#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif

#endif // __DM_THREAD_POOL_INCLUDE__
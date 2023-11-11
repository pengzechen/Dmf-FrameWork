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

#include <dmfserver/utility/dm_thread_pool.h>

// start some thread 
thread_pool_t *thread_pool_create(int thread_count) {
    thread_pool_t *pool;
    int i;

    if (thread_count <= 0) {
        return NULL;
    }

    pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        return NULL;
    }

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->notify), NULL);

    pool->task_list = NULL;
    pool->thread_list = NULL;
    // pool->timer_list = NULL;
	
    pool->thread_count = 0;
    pool->shutdown = false;

    for (i = 0; i < thread_count; i++) {
        thread_t *thread;

        thread = (thread_t*) malloc(sizeof(thread_t));
        if (thread == NULL) {
            thread_pool_destroy(pool);
            return NULL;
        }

        thread->pool = pool;
        thread->next = pool->thread_list;
        pool->thread_list = thread;
        pool->thread_count++;

        if (pthread_create(&(thread->tid), NULL, thread_func, thread) != 0) {
            thread_pool_destroy(pool);
            return NULL;
        }
    }

    return pool;
}


int dm_gettimeofday(struct timeval *tv, struct timezone *tz) {
    struct timespec ts;
    if (clock_gettime(0, &ts) != 0) {
        return -1;
    }
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
    return 0;
}


void *thread_func(void *arg) {
	
	thread_t* thread = (thread_t*) arg;
	thread_pool_t *pool = thread->pool;
	
	while (true) {
		pthread_mutex_lock(&(pool->lock));

			while ( pool->task_list == NULL && !pool->shutdown ) {
				pthread_cond_wait(&(pool->notify), &(pool->lock));
			}

			if ( pool->shutdown ) {
				pthread_mutex_unlock(&(pool->lock));
				pthread_exit(NULL);
			}

            // get first task
			task_t *task = pool->task_list;
			pool->task_list = task->next;

			dm_gettimeofday(&(thread->end_time), NULL);

		pthread_mutex_unlock(&(pool->lock));

		if (task->func != NULL) {
			task->func(task->arg);
		}

		free(task);

		// pthread_mutex_lock(&(pool->lock));

		// 	timer_t *timer = pool->timer_list;
		// 	while (timer != NULL) {
				
		// 		struct timeval current_time;
		// 		gettimeofday(&current_time, NULL);
				
		// 		if (timercmp(&(thread->end_time), &(timer->end_time), <)) {
		// 			break;
		// 		}
		// 		if (timer->func != NULL) {
		// 			timer->func(timer->arg);
		// 		}
				
		// 		timer_t *tmp = timer;
		// 		timer = timer->next;
		// 		free(tmp);
		// 	}
		// 	pool->timer_list = timer;

		// pthread_mutex_unlock(&(pool->lock));
	}
}

/*
// add a timer_t to pool timer_list 
int add_timer(thread_pool_t *pool, int seconds, void (func)(void), void *arg) {
	
	timer_t *timer;
	timer = (timer_t*) malloc(sizeof(timer_t));
	if (timer == NULL) {
		return -1;
	}

	gettimeofday(&(timer->end_time), NULL);
	timer->end_time.tv_sec += seconds;
	timer->func = func;
	timer->arg = arg;
	timer->next = NULL;

	pthread_mutex_lock(&(pool->lock));
	
	if (pool->timer_list == NULL) {
		pool->timer_list = timer;
	} else {
		timer_t *tmp = pool->timer_list;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		tmp->next = timer;
	}
	
	pthread_mutex_unlock(&(pool->lock));

	return 0;
}
*/

// add a task_t to pool thread_list 
int thread_pool_add_task(thread_pool_t *pool, void (*func)(void*), void *arg) {
    task_t *task;

    task = (task_t*) malloc(sizeof(task_t));
    if (task == NULL) {
        return -1;
    }

    task->func = func;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&(pool->lock));
	
    if (pool->task_list == NULL) {
        pool->task_list = task;
    } else {
        task_t *tmp = pool->task_list;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = task;
    }

    pthread_cond_signal(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}


static void sleep_microseconds(int microseconds) {
#ifdef _WIN32
    Sleep(microseconds / 1000);
#else
    usleep(microseconds);
#endif
}


void thread_pool_destroy(thread_pool_t *pool) {
    thread_t *thread = pool->thread_list;
	pthread_mutex_lock(&(pool->lock));
	pool->shutdown = true;
	pthread_cond_broadcast(&(pool->notify));
	pthread_mutex_unlock(&(pool->lock));

	while (thread != NULL) {
		pthread_join(thread->tid, NULL);
		thread_t *tmp = thread;
		thread = thread->next;
		free(tmp);
	}

	task_t *task = pool->task_list;
	while (task != NULL) {
		task_t *tmp = task;
		task = task->next;
		free(tmp);
	}

	// timer_t *timer = pool->timer_list;
	// while (timer != NULL) {
	// 	timer_t *tmp = timer;
	// 	timer = timer->next;
	// 	free(tmp);
	// }

	pthread_mutex_destroy(&(pool->lock));
	pthread_cond_destroy(&(pool->notify));
	free(pool);
}


int is_thread_pool_empty(thread_pool_t *pool) {
	
    pthread_mutex_lock(&(pool->lock));
	
    int is_empty = (pool->task_list == NULL);
	
    pthread_mutex_unlock(&(pool->lock));
	
    return is_empty;
}


void task_func(void *arg) {
    
    int *num = (int*) arg;
    printf("Task start, num = %d\n", *num);
    
    // sleep_microseconds((*num)*200*1000); // 模拟任务执行时间
    sleep_microseconds(3000*1000); // 模拟任务执行时间

	printf("                       Task end, num = %d\n", *num);
}


void timer_func(void *arg) {
	
    printf("Timer expired!\n");
}


// gcc thread_pool.c -I pthread -L . -lpthreadVC2 -DHAVE_STRUCT_TIMESPEC -static -o test
int test_thread_pool() {
    
    int i;
    int nums[20];
    thread_pool_t *pool = thread_pool_create(4);

    // 向线程池提交任务
    for (i = 0; i < 20; i++) {
        nums[i] = i;
        thread_pool_add_task(pool, task_func, &(nums[i]));
		// 添加定时器
	    //add_timer(pool, 1, timer_func, NULL);
    }

    // 等待所有任务执行完毕
    while (!is_thread_pool_empty(pool)) {
        sleep_microseconds(100000);
    }

    // 销毁线程池
    thread_pool_destroy(pool);

    return 0;
}

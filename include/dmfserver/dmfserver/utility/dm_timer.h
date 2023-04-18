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

#ifndef __DM_TIMER_INCLUDE__
#define __DM_TIMER_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// 类似于 usleep 的函数
void sleep_microseconds(int microseconds) {
#ifdef _WIN32
    Sleep(microseconds / 1000);
#else
    usleep(microseconds);
#endif
}

// 以下是定时器的示例代码
typedef struct Event {
    time_t timeout;
    void (*callback)();
} Event;

typedef struct MinHeap {
    Event *events[1024];
    int size;
} MinHeap;

#ifdef __cplusplus
extern "C" {
#endif
    static void min_heap_push(MinHeap *heap, Event *event);
    static Event* min_heap_top(MinHeap *heap);
    static void min_heap_pop(MinHeap *heap);
    void handle_events(MinHeap *heap);
    void add_timer(MinHeap *heap, int timeout, void (*callback)());
    

#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif


#endif // __DM_TIMER_INCLUDE__
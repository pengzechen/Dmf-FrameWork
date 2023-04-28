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

#include <dmfserver/utility/dm_timer.h>

// 类似于 usleep 的函数
void sleep_microseconds(int microseconds) {
#ifdef _WIN32
    Sleep(microseconds / 1000);
#else
    usleep(microseconds);
#endif
}

static void min_heap_push(MinHeap *heap, Event *event) {
    int i = heap->size++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap->events[p]->timeout <= event->timeout) break;
        heap->events[i] = heap->events[p];
        i = p;
    }
    heap->events[i] = event;
}

static Event* min_heap_top(MinHeap *heap) {
    return heap->events[0];
}

static void min_heap_pop(MinHeap *heap) {
    Event *event = heap->events[--heap->size];
    int i = 0;
    while (i * 2 + 1 < heap->size) {
        int a = i * 2 + 1, b = i * 2 + 2;
        if (b < heap->size && heap->events[b]->timeout < heap->events[a]->timeout) a = b;
        if (heap->events[a]->timeout >= event->timeout) break;
        heap->events[i] = heap->events[a];
        i = a;
    }
    heap->events[i] = event;
}

void handle_events(MinHeap *heap) {
    time_t current_time = time(NULL);
    while (heap->size > 0 && heap->events[0]->timeout <= current_time) {
        Event *event = min_heap_top(heap);
        min_heap_pop(heap);
        event->callback();
        free(event);
    }
}

void add_timer(MinHeap *heap, int timeout, void (*callback)()) {
    Event *event = (Event *)malloc(sizeof(Event));
    event->timeout = time(NULL) + timeout;
    event->callback = callback;
    min_heap_push(heap, event);
}



/* test function start*/
static void print_current_time() {
    time_t current_time = time(NULL);
    printf("Current time: %s", ctime(&current_time));
}

static int test_timer() {
    MinHeap heap = { .size = 0 };
    add_timer(&heap, 5, print_current_time);
    add_timer(&heap, 10, print_current_time);
    add_timer(&heap, 15, print_current_time);

    while (true) {
        handle_events(&heap);
        // 睡眠1秒，避免空循环
        sleep_microseconds(1000 * 1000);
    }
    return 0;
}

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

#include <dmfserver/utility/dm_stack.h>


// 初始化堆栈
Stack *stack_create(size_t capacity) {
    Stack *stack = (Stack*)malloc(sizeof(Stack));
    if (stack == NULL) {
        return NULL;
    }
    stack->data = (Data*)malloc(sizeof(Data) * capacity);
    if (stack->data == NULL) {
        free(stack);
        return NULL;
    }
    stack->size = 0;
    stack->capacity = capacity;
    pthread_mutex_init(&stack->mutex, NULL);
    pthread_cond_init(&stack->cond, NULL);
    return stack;
}

// 压栈
void stack_push(Stack *stack, Data data) {
    pthread_mutex_lock(&stack->mutex);
    while (stack->size == stack->capacity) {
        pthread_cond_wait(&stack->cond, &stack->mutex);
    }

    stack->data[stack->size++] = data;

    pthread_cond_signal(&stack->cond);
    pthread_mutex_unlock(&stack->mutex);
}

// 出栈
Data stack_pop(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    while (stack->size == 0) {
        pthread_cond_wait(&stack->cond, &stack->mutex);
    }

    Data data = stack->data[--stack->size];

    pthread_cond_signal(&stack->cond);
    pthread_mutex_unlock(&stack->mutex);
    return data;
}

// 获取栈顶元素
Data stack_top(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    while (stack->size == 0) {
        pthread_cond_wait(&stack->cond, &stack->mutex);
    }

    Data data = stack->data[stack->size - 1];

    pthread_cond_signal(&stack->cond);
    pthread_mutex_unlock(&stack->mutex);
    return data;
}

// 获取栈长度
size_t stack_size(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    size_t size = stack->size;
    pthread_mutex_unlock(&stack->mutex);
    return size;
}

// 销毁堆栈
void stack_destroy(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    free(stack->data);
    pthread_mutex_unlock(&stack->mutex);
    pthread_mutex_destroy(&stack->mutex);
    pthread_cond_destroy(&stack->cond);
    free(stack);
}

// 测试代码
void *producer1(void *arg) {
    Stack *stack = (Stack*)arg;
    for (int i = 1; i <= 10; i++) {
        Data data = {i};
        printf("Push: %d\n", data.value);
        stack_push(stack, data);
    }
    return NULL;
}

void *consumer1(void *arg) {
    Stack *stack = (Stack*)arg;
    for (int i = 1; i <= 10; i++) {
        Data data = stack_pop(stack);
		printf("Pop: %d\n", data.value);
	}
	return NULL;
}


int test_stack() {
	Stack *stack = stack_create(10);

	pthread_t producer_thread, consumer_thread;
	pthread_create(&producer_thread, NULL, producer1, stack);
	pthread_create(&consumer_thread, NULL, consumer1, stack);

	pthread_join(producer_thread, NULL);
	pthread_join(consumer_thread, NULL);

	stack_destroy(stack);
	return 0;
}
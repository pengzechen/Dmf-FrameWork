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

#include <dmfserver/utility/dm_queue.h>

// 初始化队列
Queue *queue_create() {
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

// 入队
void queue_enqueue(Queue *queue, Data data) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    pthread_mutex_lock(&queue->mutex);
    if (queue->head == NULL) {
        queue->head = new_node;
    } else {
        queue->tail->next = new_node;
    }
    queue->tail = new_node;
    queue->size++;

    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

// 出队
Data queue_dequeue(Queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    Node *node = queue->head;
    Data data = node->data;
    queue->head = node->next;
    queue->size--;

    free(node);
    pthread_mutex_unlock(&queue->mutex);
    return data;
}

// 获取队列长度
size_t queue_size(Queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    size_t size = queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return size;
}

// 销毁队列
void queue_destroy(Queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    Node *node = queue->head;
    while (node != NULL) {
        Node *next = node->next;
        free(node);
        node = next;
    }
    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}

// 测试代码
void *producer(void *arg) {
    Queue *queue = (Queue*)arg;
    for (int i = 1; i <= 10; i++) {
        Data data = {i};
        printf("Enqueue: %d\n", data.value);
        queue_enqueue(queue, data);
    }
    return NULL;
}

void *consumer(void *arg) {
    Queue *queue = (Queue*)arg;
    for (int i = 1; i <= 10; i++) {
        Data data = queue_dequeue(queue);
        printf("Dequeue: %d\n", data.value);
}
return NULL;
}

int test_queue() {
	Queue *queue = queue_create();
	pthread_t producer_thread, consumer_thread;
	pthread_create(&producer_thread, NULL, producer, queue);
	pthread_create(&consumer_thread, NULL, consumer, queue);

	pthread_join(producer_thread, NULL);
	pthread_join(consumer_thread, NULL);

	queue_destroy(queue);
	return 0;
}

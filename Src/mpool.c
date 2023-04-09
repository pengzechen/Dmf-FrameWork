#include <dmfserver/mpool.h>

pool_t pool;


void pool_init(int block_size, int total_size) {
    pool.block_size = block_size;
    pool.total_size = total_size;
    pool.mem_pool = malloc(total_size);
    pool.head = NULL;
    pthread_mutex_init(&pool.lock, NULL);
    pthread_cond_init(&pool.cond, NULL);
    pool.count = 0;

    char *ptr = pool.mem_pool;
    int num_blocks = total_size / block_size;
    for (int i = 0; i < num_blocks; i++) {
        node_t *node = malloc(sizeof(node_t));
        node->data = ptr;
        node->used = 0; // Set the used flag to 0
        node->next = pool.head;
        pool.head = node;
        ptr += block_size;
        pool.count++;
    }
}

void pool_destroy() {
    node_t *current = pool.head;
    while (current != NULL) {
        node_t *temp = current;
        current = current->next;
        free(temp);
    }
    free(pool.mem_pool);
    pthread_mutex_destroy(&pool.lock);
    pthread_cond_destroy(&pool.cond);
}

void *pool_alloc() {
    pthread_mutex_lock(&pool.lock);

    if ( pool.count == 0 ) {
        printf("可用为0\n");
        pthread_cond_wait(&pool.cond, &pool.lock);
    }
    printf("%d\n", pool.count);

    node_t *current = pool.head;
    
    while (current != NULL) {
        if(current->used == 0)
        break;
        current = current->next;
    }

    if (current == NULL) {
        printf(" No free block found (should not happen) \n");
        pthread_mutex_unlock(&pool.lock);
        return NULL;
    }

    current->used = 1;

    pool.count--;

    pthread_mutex_unlock(&pool.lock);
    return current->data;
}

void pool_free(void *data) {
    pthread_mutex_lock(&pool.lock);

    node_t *current = pool.head;

    while (current != NULL) {
        if(current->data == data)
        break;
        current = current->next;
    }

    if (current == NULL) {
        printf("Invalid data pointer\n");
        pthread_mutex_unlock(&pool.lock);
        return;
    }

    current->used = 0;
    pool.count++;

    pthread_cond_signal(&pool.cond);
    pthread_mutex_unlock(&pool.lock);
}

#include <dmfserver/mpool.h>

static pool_t pool;


void pool_init(int block_size, int total_size) {
    pool.block_size = block_size;
    pool.total_size = total_size;
    pool.mem_pool = malloc(total_size);
    pool.head = NULL;
    pthread_mutex_init(&pool.lock, NULL);
    pthread_cond_init(&pool.cond, NULL);
    pool.count = 0;

    // Divide the memory pool into blocks and add them to the free list
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

    if ( pool.count == 0) {
        // Wait for a block to become available
        pthread_cond_wait(&pool.cond, &pool.lock);
    }

    // Find the first free block
    node_t *current = pool.head;
    while (current != NULL && current->used != 0) {
        current = current->next;
    }

    if (current == NULL) {
        // No free block found (should not happen)
        pthread_mutex_unlock(&pool.lock);
        return NULL;
    }

    // Mark the block as used
    current->used = 1;

    // Remove the block from the list
    if (current == pool.head) {
        pool.head = current->next;
    } else {
        node_t *prev = pool.head;
        while (prev != NULL && prev->next != current) {
            prev = prev->next;
        }
        prev->next = current->next;
    }
    pool.count--;

    pthread_mutex_unlock(&pool.lock);

    // Return the data from the block
    return current->data;
}

void pool_free(void *data) {
    pthread_mutex_lock(&pool.lock);

    // Find the node corresponding to the data
    node_t *current = pool.head;
    while (current != NULL && current->data != data) {
        current = current->next;
    }

    if (current == NULL) {
        // Invalid data pointer (should not happen)
        pthread_mutex_unlock(&pool.lock);
        return;
    }

    // Mark the block as free
    current->used = 0;

    // Add the block back to the list
    current->next = pool.head;
    pool.head = current;
    pool.count++;

    // Signal a waiting thread that a block is available
    pthread_cond_signal(&pool.cond);
    pthread_mutex_unlock(&pool.lock);
}

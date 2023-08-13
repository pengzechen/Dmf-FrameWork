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

#include <dmfserver/utility/dm_map.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


// 初始化哈希映射
hashmap_t *hashmap_create(size_t size) {
    hashmap_t *hashmap = (hashmap_t *)malloc(sizeof(hashmap_t));
    if (hashmap == NULL) {
        return NULL;
    }

    hashmap->size = size;
    hashmap->buckets = (hashmap_node_t **)calloc(size, sizeof(hashmap_node_t *));
    if (hashmap->buckets == NULL) {
        free(hashmap);
        return NULL;
    }

    return hashmap;
}

// 哈希函数示例（简化版）
size_t hash_function(const char *key, size_t size) {
    size_t hash = 0;
    while (*key) {
        hash = (hash * 31) + *key;
        key++;
    }
    return hash % size;
}

// 插入键值对到哈希映射
int hashmap_insert(hashmap_t *hashmap, const char *key, void *value) {
    size_t index = hash_function(key, hashmap->size);

    hashmap_node_t *new_node = (hashmap_node_t *)malloc(sizeof(hashmap_node_t));
    if (new_node == NULL) {
        return -1;
    }

    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = hashmap->buckets[index];
    hashmap->buckets[index] = new_node;

    return 0;
}

// 获取指定键的值
void *hashmap_get(hashmap_t *hashmap, const char *key) {
    size_t index = hash_function(key, hashmap->size);

    hashmap_node_t *node = hashmap->buckets[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

// 删除指定键的节点
int hashmap_remove(hashmap_t *hashmap, const char *key) {
    size_t index = hash_function(key, hashmap->size);

    hashmap_node_t *prev = NULL;
    hashmap_node_t *curr = hashmap->buckets[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (prev == NULL) {
                hashmap->buckets[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->key);
            free(curr);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    return -1;
}

// 释放哈希映射的内存
void hashmap_destroy(hashmap_t *hashmap) {
    for (size_t i = 0; i < hashmap->size; i++) {
        hashmap_node_t *node = hashmap->buckets[i];
        while (node != NULL) {
            hashmap_node_t *temp = node;
            node = node->next;
            free(temp->key);
            free(temp);
        }
    }
    free(hashmap->buckets);
    free(hashmap);
}

int main() {
    hashmap_t *hashmap = hashmap_create(100);

    hashmap_insert(hashmap, "key1", "value1");
    hashmap_insert(hashmap, "key2", "value2");

    void *value1 = hashmap_get(hashmap, "key1");
    if (value1 != NULL) {
        printf("Value for key1: %s\n", (char *)value1);
    }

    hashmap_remove(hashmap, "key1");

    hashmap_destroy(hashmap);

    return 0;
}

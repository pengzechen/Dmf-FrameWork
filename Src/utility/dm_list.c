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

#include <dmfserver/utility/dm_list.h>

// 初始化列表
List *list_create() {
    List *list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

// 添加元素
void list_append(List *list, Data data) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = new_node;
    } else {
        Node *node = list->head;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = new_node;
    }
    list->size++;
}

// 获取元素数量
size_t list_size(List *list) {
    return list->size;
}

// 查找元素，若找到返回true
bool list_find(List *list, Data data) {
    Node *node = list->head;
    while (node != NULL) {
        if (node->data.value == data.value) {
            return true;
        }
        node = node->next;
    }
    return false;
}

// 销毁列表
void list_destroy(List *list) {
    Node *node = list->head;
    while (node != NULL) {
        Node *next = node->next;
        free(node);
        node = next;
    }
    free(list);
}



/* test function start*/
int test_list() {
    List *list = list_create();

    Data data1 = {1};
    list_append(list, data1);

    Data data2 = {2};
    list_append(list, data2);

    Data data3 = {3};
    list_append(list, data3);

    printf("List size: %lu\n", list_size(list));
    printf("Find 2 in list: %s\n", list_find(list, data2) ? "true" : "false");
    printf("Find 4 in list: %s\n", list_find(list, (Data){4}) ? "true" : "false");

    list_destroy(list);
    return 0;
}

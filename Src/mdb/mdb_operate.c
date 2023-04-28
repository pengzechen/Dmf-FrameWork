
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

#include <dmfserver/mdb/mdb_operate.h>


static HANDLE file_mapping;
static HANDLE mutex;


void mdb_operate_init() 
{
    file_mapping = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        MAX_ENTRIES * sizeof(entry_t),
        SHARED_STR
    );
    if (file_mapping == NULL) {
        printf("Failed to create file mapping object. Error code: %d\n", GetLastError());
        return ;
    }

    mutex = CreateMutex(NULL, FALSE, SHARED_MUTEX);
    if (mutex == NULL) {
        printf("Failed to create mutex. Error code: %d\n", GetLastError());
        // UnmapViewOfFile(shared_data);
        CloseHandle(file_mapping);
        return ;
    }
}

char* mdb_find(char* key)
{

    entry_t *shared_data;
    int flag = 0;
    char* res;
    int i = 0;


    shared_data = (entry_t *)MapViewOfFile(
        file_mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        MAX_ENTRIES * sizeof(entry_t)
    );

    if (shared_data == NULL) {
        printf("Failed to map view of file. Error code: %d\n", GetLastError());
        //CloseHandle(file_mapping);
        return "";
    }

    WaitForSingleObject(mutex, INFINITE);
    for (; i < MAX_ENTRIES; i++) {
        if (strcmp(shared_data[i].key, key) == 0) {
            flag = 1;
            break;
        }
    }
    ReleaseMutex(mutex);

    if(flag) {
        res =  (char*)malloc(sizeof(char)*MAX_VALUE_LEN) ;
        strcpy(res, shared_data[i].value);
        UnmapViewOfFile(shared_data);
        return res;
    }
    
    UnmapViewOfFile(shared_data);
    return "";
    
}
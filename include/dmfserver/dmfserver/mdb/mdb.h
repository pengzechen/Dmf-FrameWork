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

#ifndef __MDB_INCLUDE__
#define __MDB_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN 1024
#define MAX_ENTRIES 65535

#define SHARED_STR      "SharedMemory"
#define SHARED_MUTEX    "SharedMemoryMutex"

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} entry_t;



#endif // __MDB_INCLUDE__
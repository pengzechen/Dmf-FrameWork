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
#ifndef __Mdb_INCLUDE__
#define __Mdb_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_KEY_LEN 512
#define MAX_VALUE_LEN 2048
#define MAX_ENTRIES 1024
#define SHARED_STR "SharedMemory"
#define SHARED_MUTEX "SharedMemoryMutex"

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} entry_t;


#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

#ifdef __WIN32__
	extern void mdb_find(char* key);
#elif __linux__			// WIN32
	
#endif // linux


#ifdef __cplusplus
}    /* end of the 'extern "C"' block */
#endif

#endif // __Mdb_INCLUDE__
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

#ifndef __MDB_OPERATE_INCLUDE__
#define __MDB_OPERATE_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dmfserver/utility/dm_hash.h>
#include <time.h>

#ifdef __WIN32__
	#define DLL_EXPORT __declspec(dllexport)
	#include <windows.h>
#endif // WIN32

#ifdef __linux__
    #include <sys/ipc.h>
    #include <sys/shm.h>
#endif // LINUX

#include "mdb.h"


#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

    #ifdef __WIN32__
    
        DLL_EXPORT extern void mdb_operate_init();

        DLL_EXPORT extern char* mdb_find(char* key);

        DLL_EXPORT extern void mdb_insert(char* key, char* value);

        DLL_EXPORT extern int ip_check_valid(char* ip);
        
    #elif __linux__			// WIN32
        extern void mdb_operate_init();

        extern char* mdb_find(char* key);

        extern void mdb_insert(char* key, char* value);
    #endif // linux


#ifdef __cplusplus
}    /* end of the 'extern "C"' block */
#endif

#endif // __MDB_OPERATE_INCLUDE__
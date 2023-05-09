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

#ifdef __WIN32__
    #include <windows.h>
#endif // __WIN32__



/*      cache Shared Memory       */
    #define SHARED_STR      "cache_shared_memory"
    #define SHARED_MUTEX    "cache_shared_memory_mutex"
    #define MAX_ENTRIES 65535

    #define MAX_KEY_LEN 64
    #define MAX_VALUE_LEN 512
    typedef struct {
        int op;
        char key[MAX_KEY_LEN];
        char value[MAX_VALUE_LEN];
    } entry_t;
/*      cache Shared Memory       */





/*      Middle1 Shared Memory       */
#define IP_LIMIT_SHARED_STR     "ip_limit_shared_memory"
#define IP_LIMIT_MUTEX          "ip_limit_shared_memory_mutex"
#define MAX_IP_LIMIT_HANDLE 10240

typedef struct _IpLimit {
    char    ip[16];
    int     time_start;
    int     request_num;
    int     invalid_num;
    int     ab_ban;
} IpLimit;
/*      Middle1 Shared Memory       */

#endif // __MDB_INCLUDE__
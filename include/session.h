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
#ifndef SESSION
#define SESSION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <request.h>
#include <windows.h>

#define HASH_DEC_LEN 5

typedef struct _SessionData {
    char* key;
    char* data;
    int time_store;
    int expire;
    struct _SessionData* next;
} SessionData;

// Hash Map defination 
typedef struct _HashNode
{
    char* key;
    SessionData* value;
    struct _HashNode* next; // 当下标相同时，指向集合中的下一个节点

} HashNode ;

#define SESSION_EXPIRE_DEFAULT 60 * 2

// 在session模块中使用的全局变量
static HashNode g_session_all_dec[ HASH_DEC_LEN ];

#ifdef __cplusplus
extern "C" {
#endif

    extern void SessionInit();

    extern void SessionCreate(char* ,char*, char*);

    extern void SessionAll();

    extern char* getSessionR(const Request* req, char* key);

    extern int SessionAddR(const Request* req, char* key, char* data);

    extern int UpdateSessionDataR(const Request* req, char* key, char* newdata);

    extern char* getSession(char* session_str, char* key);

    extern int SessionAdd(char* session_str, char* key, char* value);

    extern int UpdateSessionData(char* session_str, char* key, char* newdata);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif /* SESSION */
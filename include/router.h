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
#ifndef ROUTER
#define ROUTER

#define STATIC_DIR "./static"
#define STATIC_FILES_MAX_NUM 128

#include <request.h>	// Router要接受req并把req传给 control function
#include <response.h>   // Router找不到资源时直接调用 response 返回
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define ContFunNUM 60

typedef void (*ContFun)(int acceptFd, const Request *req );

typedef struct _ContFunMap {
	
	ContFun cf[ ContFunNUM ];
	char* keys[ ContFunNUM ];
	int curr_num;
	
} ContFunMap;

// 全局 view 回调函数
extern ContFunMap g_cmp;


#define RouterAdd(name) void name()


#ifdef __cplusplus
extern "C" {
#endif

void Router_init();

void Rou_init( ContFun cf[], char* keys[], int acceptFd, Request *req);

void Rou_iocp_init(ContFunMap cmp, int acceptFd, Request *req);

static int searchLocalFile(char* local_paths[]);

static char* loadFile(char *path);

void router_add_app(ContFun cf[], char* keys[], const char* name);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif
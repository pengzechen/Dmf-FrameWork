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

#ifndef __ROUTER_INCLUDE__
#define __ROUTER_INCLUDE__

#define STATIC_DIR "./static"
#define STATIC_FILES_MAX_NUM 128

#include <dmfserver/request.h>	// Router要接受req并把req传给 control function
#include <dmfserver/response.h>   // Router找不到资源时直接调用 response 返回
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>


#define MAX_PATH_LENGTH 1024
#define MAX_FILES 500
#define ContFunNUM 512

#define RED "\033[0;32;31m"
#define NONE "\033[m"
#define YELLOW "\033[1;33m"



// #define Router_Debug


struct FileInfo {
    char path[MAX_PATH_LENGTH];
    char type[16];
    off_t size;
    char ext[16];
    char content_type[64];
    char url[512];
};

typedef void (*ContFun)(int acceptFd, const Request *req );

#define RouterAdd(name) void name()

typedef struct _ContFunMap {
	
	ContFun cf[ ContFunNUM ];
	char* keys[ ContFunNUM ];
	int curr_num;
	
} ctl_fun_map_t;

// 全局 view 回调函数
extern ctl_fun_map_t g_cmp;

extern struct FileInfo g_file_list[MAX_FILES];
extern int g_num_files;

#ifdef __cplusplus
extern "C" {
#endif

void router_init();

void router_handle(int acceptFd, Request *req);

static int search_local_file(char* local_paths[]);

void traverse_directory(const char *path, struct FileInfo file_list[], int *num_files);

char* get_content_type(char *file_ext);

void router_add_app(ContFun cf[], char* keys[], const char* name);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __ROUTER_INCLUDE__
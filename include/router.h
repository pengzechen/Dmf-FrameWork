
#ifndef ROUTER
#define ROUTER

#define STATIC_DIR "./static"
#define STATIC_FILES_MAX_NUM 128

#include "request.h"	// Router要接受req并把req传给 control function
#include "response.h"  // Router找不到资源时直接调用 response 返回
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>



typedef void (*ContFun)(int acceptFd, const Request *req );

typedef struct _ContFunMap {
	
	ContFun cf[20];
	char* keys[20];
	
} ContFunMap;

void Rou_init( ContFun cf[], char* keys[], int num, int acceptFd, Request *req);

void Rou_iocp_init(ContFunMap cmp, int acceptFd, Request *req);

static int searchLocalFile(char* local_paths[]);

static char* loadFile(char *path);

#endif
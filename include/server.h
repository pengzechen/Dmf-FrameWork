
#ifndef SERVER
#define SERVER

#define SERVER_PORT 80
#define RECEIVE_MAX_BYTES 1024*1024

#include "request.h"
#include "template.h"
#include "response.h"
#include "router.h"
#include "model.h"
#include "utility.h"

#pragma comment(lib, "ws2_32.lib")      //windows下调用动态库
#include<WinSock2.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

static void Handler(int acceptFd, ContFun cf[], char* keys[]);

extern void SimpleServerMake(ContFun cf[], char* keys[]);

#endif
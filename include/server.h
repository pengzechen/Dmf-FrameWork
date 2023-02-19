
#ifndef SERVER
#define SERVER

#define SERVER_PORT 80
#define RECEIVE_MAX_BYTES 1024*1024

#define DATA_BUFSIZE 8192
#define OutErr(a) printf("%s %s %d %d", a, WSAGetLastError(), __FILE__ , __LINE__ );
#define OutMsg(a) printf("%s", a);

#include "request.h"
#include "template.h"
#include "response.h"
#include "router.h"
#include "session.h"
#include "model.h"
#include "utility.h"
#include "cpool.h"

#pragma comment(lib, "ws2_32.lib")      //windows下调用动态库
#include<WinSock2.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

 

typedef struct {
	
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[DATA_BUFSIZE];
	ContFunMap cmp;
	
}PER_IO_OPERATION_DATA,* LPPER_IO_OPERATION_DATA;
 
 
typedef struct {
	
	SOCKET Socket;
	
}PER_HANDLE_DATA,* LPPER_HANDLE_DATA;



static void Handler(int acceptFd, ContFun cf[], char* keys[]);

extern void SimpleServerMake(ContFun cf[], char* keys[]);

extern int iocpServerMake(ContFunMap cmp);

#endif
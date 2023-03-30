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
#ifndef SERVER
#define SERVER

#define SERVER_PORT 80
#define RECEIVE_MAX_BYTES 1024*1024

#define DATA_BUFSIZE 8192

#define MAXBUF 4096

#define OutErr(a) printf("%s %s %d %d", a, WSAGetLastError(), __FILE__ , __LINE__ );
#define OutMsg(a) printf("%s", a);


#include <conf/conf.h>			// 初始化 全局配置
#include <utility.h>	
#include <router.h>				// router 中已经有 server 需要的 request 和 response

#include <stdio.h>
#include <string.h>
#include <time.h>


#include <openssl/ssl.h>
#include <openssl/err.h>
// #include <openssl/applink.c>


#ifdef __WIN32__

#include <WinSock2.h>

typedef struct {
	
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[DATA_BUFSIZE];
	ContFunMap cmp;
	
}PER_IO_OPERATION_DATA,* LPPER_IO_OPERATION_DATA;
 
 
typedef struct {
	
	SOCKET Socket;
	
}PER_HANDLE_DATA,* LPPER_HANDLE_DATA;


#ifdef __cplusplus
extern "C" {
#endif
	
	static void Handler(int acceptFd, ContFun cf[], char* keys[]);

	extern void SimpleServerMake(ContFun cf[], char* keys[]);

	extern int iocpServerMake(ContFunMap cmp);

	extern void SSLservermake(ContFun cf[], char* keys[]);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#elif __linux__

#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	static void Handler(int acceptFd, ContFun cf[], char* keys[]);

	extern void SimpleServerMake(ContFun cf[], char* keys[]);

	extern void SSLservermake(ContFun cf[], char* keys[]);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif  // Windows or Linux

#endif  // SERVER
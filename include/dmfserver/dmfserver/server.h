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


#include <dmfserver/conf/conf.h>			// 初始化 全局配置
#include <dmfserver/utility.h>	
#include <dmfserver/router.h>				// router 中已经有 server 需要的 request 和 response
#include <dmfserver/log.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
// #include <openssl/applink.c>

#define OutErr(a) printf("%s %s %d \n", a, __FILE__ , __LINE__ );


#ifdef __linux__ // Linux

	#include <stdarg.h>
	#include <assert.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/epoll.h>
	#include <openssl/crypto.h>
	#include <openssl/rand.h>

	typedef struct fd_ssl_map {
		int fd;
		SSL* ssl;
		struct fd_ssl_map* next;
	} fd_ssl_map ;

#endif  		// Linux


#ifdef __WIN32__ // Windows
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

#endif  		// Windows


#ifdef __cplusplus
extern "C" {
#endif

	extern void handle_signal(int sig_num);
	
	static void Handler(int acceptFd, ContFun cf[], char* keys[]);

	extern void SimpleServerMake(ContFun cf[], char* keys[]);

	void SSLservermake(ContFun cf[], char* keys[]);

	#ifdef __WIN32__   // Windows IOCP Model
		extern int iocpServerMake(ContFunMap cmp);
	#endif  		   // Windows
	
	#ifdef __linux__   // linux epool Model
		extern int threadingServerRunning();
		extern int epool_ssl_server();
	#endif  		   // linux

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif  // SERVER
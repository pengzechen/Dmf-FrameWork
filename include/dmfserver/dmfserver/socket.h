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

#ifndef __SOCKET_INCLUDE__
#define __SOCKET_INCLUDE__

#include <dmfserver/conf/conf.h>			// 初始化 全局配置

#include <stdio.h>
#include <string.h>

#ifdef __WIN32__

#include <WinSock2.h>
#include <WS2tcpip.h>

#else

#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#endif // __WIN32__

#define SERVER_PORT 8080
#define OutErr(a) printf("%s %s %d \n", a, __FILE__ , __LINE__ );

#ifdef __WIN32__
#define close_socket closesocket
#else
#define close_socket close
#endif // __WIN32__

#ifdef __cplusplus
extern "C" {
#endif

	extern int create_socket();
    extern int createSocket();
    extern int create_socket_reuse();

#ifdef __WIN32__

static inline void wsa_init() {
    WSADATA wsd;
    if( WSAStartup(MAKEWORD(2, 2), &wsd) != 0)  
        OutErr("WSAStartup()");
}

static inline void wsa_cleanup() {
    WSACleanup(); 
}


SOCKET iocp_bind_socket(int nPort);
    
#endif


#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif  // __SOCKET_INCLUDE__
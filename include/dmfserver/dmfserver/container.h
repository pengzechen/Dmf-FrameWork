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

#ifndef __SERVER_INCLUDE__
#define __SERVER_INCLUDE__


#define RECEIVE_MAX_BYTES 1024*512
#define MAXBUF 4096


#include <dmfserver/conf/conf.h>			// 初始化 全局配置
#include <dmfserver/utility/utility.h>	
#include <dmfserver/router.h>				// router 中已经有 server 需要的 request 和 response
#include <dmfserver/log.h>
#include <dmfserver/mpool.h>
#include <dmfserver/middleware/middleware.h>
#include <dmfserver/socket.h>

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
#include <sys/time.h>




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


typedef struct thread_arg {
	ctl_fun_map_t cmp;
	long fd;
} thread_arg;

#ifdef __cplusplus
extern "C" {
#endif

static SSL_CTX* get_ssl_ctx();

static void simple_container_handler(connection_tp conn_ptr );

extern void simple_container_make();

extern void simple_ssl_container_make();

extern void container_init ();

extern void container_start ();

#ifdef __WIN32__   // Windows IOCP Model
extern int iocp_container_make();
#endif  		   // Windows

#ifdef __linux__   // linux epool Model
extern void epoll_container_make();
extern int epoll_ssl_container();
#endif  		   // linux

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif  // __SERVER_INCLUDE__
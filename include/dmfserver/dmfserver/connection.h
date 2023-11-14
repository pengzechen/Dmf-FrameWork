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

#ifndef __CONNECTION_INCLUDE__
#define __CONNECTION_INCLUDE__
#ifdef __linux__
#include <sys/epoll.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dmfserver/request.h>

#define DATA_BUFSIZE 2048

#ifdef __WIN32__ // Windows
#include <WinSock2.h>
#include <WS2tcpip.h>

typedef struct {
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[ DATA_BUFSIZE ];
} per_io_data_t, * per_io_data_tp;

typedef struct {
	SOCKET Socket;
    struct _connection_t * conn;
} per_handle_data_t, * per_handle_data_tp;

#else  		// Windows

typedef struct {
	char Buffer[ DATA_BUFSIZE ];
} per_io_data_t, * per_io_data_tp;

typedef struct {
    int efd;
	int Socket;
    struct _connection_t * conn;
} per_handle_data_t, * per_handle_data_tp;

#endif // linux

typedef struct _connection_t {
    per_io_data_tp      per_io_data;
    per_handle_data_tp  per_handle_data;
    Request             *req;
} connection_t, * connection_tp;


#ifdef __cplusplus
extern "C" {
#endif

extern connection_tp 
new_connection ();

#ifdef __WIN32__
extern void
send_next (connection_tp conn) ;
#endif

extern void 
connection_close (connection_tp conn);

extern void
connection_free_base (connection_tp conn);

extern void
connection_free (connection_tp conn);


#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif  // __CONNECTION_INCLUDE__
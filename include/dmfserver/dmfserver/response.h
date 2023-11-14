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

#ifndef __RESPONSE_INCLUDE__
#define __RESPONSE_INCLUDE__

#define FINAL_STR_SIZE 1024*1024

#include <dmfserver/conf/conf.h>
#include <dmfserver/template.h>					// 以模板作为响应
#include <dmfserver/utility/utility.h>        	// 引入时间
#include <dmfserver/connection.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>



#ifdef __WIN32__

#include <WinSock2.h>		// 为了使用 send

#elif __linux__

#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>         // for close

#endif

typedef struct _Response {
	char Server[32];
	char Date[32];
	char Head_code[32];			// 200 404 ...
	char Content_type[32];
	char Set_cookie[32];	
	char Connection[32];
	char * pbody;
	unsigned int body_size;
	connection_tp conn;
}response_t;

#ifdef __cplusplus
extern "C" {
#endif

void res_init( connection_tp conn, response_t* res);

static void res_handle( connection_tp conn, char* res_str, unsigned int size);

extern void res_without_permission( connection_tp conn);

static char * res_load_file( char *path);

extern void res_set_head( response_t* res, char* code);

extern void res_set_type( response_t* res, char* type);

extern void res_set_cookie( response_t* res, char* name, char* value);

extern void res_set_session( response_t*res , char* Session_str);

extern void res_set_body( response_t* res, char* body, unsigned int size);

extern void res_parse_send( response_t* res);

extern void res_notfound( connection_tp conn);

extern void res_row(  connection_tp conn, char* res_str);

extern void res_render( connection_tp conn, char* template_name, struct Kvmap *kv, int num);

extern void res_static( connection_tp conn, char* path, unsigned int size, char* ext, char* content_type);

static void res_file_handle( connection_tp conn, char* path, char* content_type, unsigned int size);



#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __RESPONSE_INCLUDE__
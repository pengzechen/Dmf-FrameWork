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
#ifndef RESPONSE
#define RESPONSE

#define FINAL_STR_SIZE 1024*1024

#include <dmfserver/conf/conf.h>
#include <dmfserver/template.h>		// 以模板作为响应
#include <dmfserver/utility/utility.h>        // 引入时间
#include <string.h>
#include <stdio.h>

#include <openssl/ssl.h>

#ifdef __WIN32__
#include <WinSock2.h>		// 为了使用 send
#elif __linux__
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h> // for close
#endif

typedef struct _Response {
	char Server[32];
	char Date[32];
	char Head_code[32];			// 200 404 ...
	char Content_type[32];
	char Set_cookie[32];	
	char Connection[32];
	char* pbody;
	unsigned int body_size;
	int fd;
}Response;

#ifdef __cplusplus
extern "C" {
#endif

void Res_init(int fd, Response* res);

static void ResHandel( int acceptFd, char* res_str, unsigned int size);

static char* loadFile(char *path);

extern void SetHead(Response* res, char* code);

extern void SetType(Response* res, char* type);

extern void SetCookie(Response* res, char* name, char* value);

extern void SetSession(Response*res , char* Session_str);

extern void SetBody(Response* res, char* body, unsigned int size);

extern void ResParseSend(Response* res);

extern void Res_NotFound(int acceptFd);

extern void Res_row( int acceptFd, char* res_str);

extern void Res_render( int acceptFd, char* path, struct Kvmap *kv, int num);

extern void Res_static(int acceptFd, char* path, unsigned int size, char* ext, char* content_type);

static void ResFileHandel(int acceptFd, char* path, char* content_type, unsigned int size);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif
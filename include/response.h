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

#define FINAL_STR_SIZE 4096*5


#include "template.h"		// 以模板作为响应
#include "session.h"
#include "utility.h"        // 引入时间
#include <string.h>
#include <stdio.h>
#include <WinSock2.h>		// 为了使用 send

#include <openssl/ssl.h>    // 为了使用 ssl_write
#include "conf/conf.h"

typedef struct _Response {
	char Server[32];
	char Date[32];
	char Head_code[32];			// 200 404 ...
	char Content_type[32];
	char Set_cookie[32];	
	char Connection[32];
	char body[2048];
	
	int fd;
}Response;

#ifdef __cplusplus
extern "C" {
#endif

void Res_init(int fd, Response* res);

static void ResHandel( int acceptFd, char* res_str );

extern void SetHead(Response* res, char* code);

extern void SetType(Response* res, char* type);

extern void SetCookie(Response* res, char* name, char* value);

extern void SetSession(Response*res , char* key, char* value);

extern void SetBody(Response* res, char* body);

extern void ResParse(Response* res);

extern void Res_row( int acceptFd, char* res_str);

extern void Res_NotFound(int acceptFd);

extern void Res_render( int acceptFd, char* path, struct Kvmap *kv, int num);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif
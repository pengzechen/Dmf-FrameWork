
#ifndef RESPONSE
#define RESPONSE

#define FINAL_STR_SIZE 4096


#include "template.h"		// 以模板作为响应
#include "utility.h"      // 引入时间
#include <string.h>
#include <stdio.h>
#include<WinSock2.h>		// 为了使用 send

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

void Res_init(int fd, Response* res);

static void ResHandel( int acceptFd, char* res_str );

void SetHead(Response* res, char* code);

void SetType(Response* res, char* type);

void SetCookie(Response* res, char* name, char* value);

void SetBody(Response* res, char* body);

void ResParse(Response* res);

void Res_row( int acceptFd, char* res_str);

void Res_NotFound(int acceptFd);

void Res_render( int acceptFd, char* path, struct Kvmap *kv, int num);

#endif
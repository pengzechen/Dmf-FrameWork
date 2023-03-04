
#ifndef RESPONSE
#define RESPONSE

#define FINAL_STR_SIZE 4096*5


#include "template.h"		// 以模板作为响应
#include "session.h"
#include "utility.h"        // 引入时间
#include <string.h>
#include <stdio.h>
#include <WinSock2.h>		// 为了使用 send

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

extern void SetHead(Response* res, char* code);

extern void SetType(Response* res, char* type);

extern void SetCookie(Response* res, char* name, char* value);

extern void SetSession(Response*res , char* key, char* value);

extern void SetBody(Response* res, char* body);

extern void ResParse(Response* res);

extern void Res_row( int acceptFd, char* res_str);

extern void Res_NotFound(int acceptFd);

extern void Res_render( int acceptFd, char* path, struct Kvmap *kv, int num);

#endif
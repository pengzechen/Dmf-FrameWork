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
#include "response.h"
#include <windows.h>

extern void Res_init(int fd, Response* res){
	memset(res->Server, 0, 32);
	memset(res->Content_type, 0, 32);
	memset(res->Date, 0, 32);
	memset(res->Set_cookie, 0, 32);
	memset(res->Connection, 0, 32);
	memset(res->Head_code, 0, 32);
	res->pbody = NULL;

	strcat(res->Server, "Server:DmfServer\r\n");
	
	char time_str[32] = {'\0'};
	serverTime(time_str);
	strcat(res->Date, "Date:");
	strcat(res->Date, time_str);
	strcat(res->Date, "\r\n");
	
	strcat(res->Connection, "Connection:keep-alive");
	strcat(res->Connection, "\r\n");
	
	res->fd = fd;
}

// Response 模块最后调用此函数  发送并关闭此次TCP连接
static void ResHandel( int acceptFd, char* res_str, unsigned int size) {
	int sendbyets = send(acceptFd, res_str, size, 0);

	// printf("[Response: ] Send: %d byets \n", sendbyets);
	closesocket(acceptFd);
}

// 大文件调用此模块进行返回 
static void ResFileHandel(int acceptFd, char* path, char* content_type, unsigned int size) {
	char head[512] = {0};
	strcat(head, "HTTP/1.1 200 OK\r\n");
	strcat(head, "Content-Type: ");
	strcat(head, content_type);
	strcat(head, "\r\n");
	strcat(head, "Transfer-Encoding: chunked\r\n\r\n");

	send(acceptFd, head, strlen(head), 0);

	FILE* fp;
	long long int read_size = 0;
	long long int all_size = 0;
	long long int file_size = 0;
	char buffer[1024*512];
	char chunk_header[10];
	int chunk_header_len = 0;

	fp = fopen(path, "rb");
	if(fp == NULL){
		closesocket(acceptFd);
		return;
	}

	fseek(fp, 0L, 2);
	file_size = ftell(fp);
	fseek(fp, 0L, 0);
	
	while( !feof(fp) ){
		read_size = fread(buffer, 1 , sizeof(buffer), fp);
		sprintf(chunk_header, "%x\r\n", read_size);
		chunk_header_len = strlen(chunk_header);
		send(acceptFd, chunk_header, chunk_header_len, 0);
		send(acceptFd, buffer, read_size, 0);
		send(acceptFd, "\r\n", 2, 0);
		Sleep(20);
		all_size = all_size + read_size;
	}
	fclose(fp);

	send(acceptFd, "0\r\n\r\n", 5, 0);
	// printf("%d, %ld \n", size, file_size);
	// printf("%d, %ld \n", size, all_size);
	closesocket(acceptFd);

}

// 设置 响应代码（首行）
extern void SetHead(Response* res, char* code){	
	strcat(res->Head_code, "HTTP/1.1 ");
	strcat(res->Head_code, code);
	strcat(res->Head_code, "\r\n");
}

// 设置 Content-type
extern void SetType(Response* res, char* type){
	strcat(res->Content_type, "Content-type:");
	strcat(res->Content_type, type);
	strcat(res->Content_type, "\r\n");
}

// 设置 Set-cookie 
extern void SetCookie(Response* res, char* name, char* value){
	strcat(res->Set_cookie, "Set-cookie:");
	strcat(res->Set_cookie, name);
	strcat(res->Set_cookie, "=");
	strcat(res->Set_cookie, value);
	strcat(res->Set_cookie, "\r\n");
}

// 设置 session
extern void SetSession(Response*res , char* Session_str) {
	
	SetCookie(res, "dmfsession", Session_str);
}

// 设置 body
extern void SetBody(Response* res, char* body, unsigned int size){
	res->pbody = body;
	res->body_size = size;
}

// 将结构体中的变量组合成字符串 发送
extern void ResParse(Response* res){
	char* final_str = malloc(sizeof(char)* FINAL_STR_SIZE);
	memset(final_str, 0, FINAL_STR_SIZE);

	strcat(final_str, res->Head_code);
	strcat(final_str, res->Server);
	strcat(final_str, res->Date);
	strcat(final_str, res->Content_type);
	strcat(final_str, res->Set_cookie);
	strcat(final_str, res->Connection);
	strcat(final_str, "\r\n");

	int head_len = strlen(final_str);
	memcpy(final_str + head_len, res->pbody, res->body_size);

	// printf("%d\n", res->body_size);
	
	ResHandel(res->fd, final_str, head_len + res->body_size);

	free(res->pbody);
	free(final_str);
}


// 以纯的字符串返回
extern void Res_row(int acceptFd, char* res_str) {
	char* final_str = malloc(sizeof(char)* FINAL_STR_SIZE);
	strcat( final_str, "HTTP/1.1 200 \r\nContent-type:text/html;utf-8;\r\n\r\n" );
	strcat( final_str, res_str);
	
	ResHandel(acceptFd, final_str, strlen(final_str));
	free(final_str);
}

// 返回 Not Found
extern void Res_NotFound(int acceptFd){
	char* final_str = malloc(sizeof(char)* FINAL_STR_SIZE);
	strcat( final_str, "HTTP/1.1 404 \r\nContent-type:text/html;utf-8;\r\n\r\n" );
	strcat( final_str, "Not Found");
	
	ResHandel(acceptFd, final_str, strlen(final_str));
	free(final_str);
}

// 以模板返回
extern void Res_render(int acceptFd, char* path, struct Kvmap *kv, int num) {
	char* context = loadTemplate(path);					// 需要释放内存
	char* res = parseContext(context, kv, num-1);		// 模板返回值
	Res_row(acceptFd, res);
	free(res);
}


extern void Res_static(int acceptFd, char* path, unsigned int size, char* ext, char* content_type) {
	if(size > 1024*1024*2) {				//  文件大于 2Mb 调用文件handle
		ResFileHandel(acceptFd, path, content_type, size);
		return;
	}
	char* res_str = loadFile(path);
	Response res;
	Res_init(acceptFd, &res);
	SetHead(&res, "200");
	SetType(&res, content_type);
	SetBody(&res, res_str, size);
	ResParse(&res);
	free(res_str);
}


static char* loadFile(char *path) {
	FILE *fp;
	fp = fopen( path, "rb" );
	if(fp == NULL){ 
		printf("[Response: ]%s open Failed\n", path);  
		return "";
	}
	unsigned long int file_size;
	fseek(fp, 0L, 2);
	file_size = ftell(fp);
	fseek(fp, 0L, 0);
	
	char * _context = (char*)malloc(file_size * sizeof(char)); 
	
	memset(_context, '\0', file_size * sizeof(char));
	fread(_context, sizeof(char), file_size, fp);
	fclose(fp);
	
	return _context;
}
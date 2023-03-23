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

extern void Res_init(int fd, Response* res){
	memset(res->Server, 0, 32);
	memset(res->Content_type, 0, 32);
	memset(res->Date, 0, 32);
	memset(res->Set_cookie, 0, 32);
	memset(res->Connection, 0, 32);
	memset(res->Head_code, 0, 32);
	memset(res->body, 0, 2048);

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
static void ResHandel( int acceptFd, char* res_str ){
	send(acceptFd, res_str, strlen(res_str),0);
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

extern void SetSession(Response*res , char* key, char* value){
	char Session_str[11] = {'\0'};
	SessionCreate(Session_str, key, value);
	SetCookie(res, "dmfsession", Session_str);
}


// 设置 body
extern void SetBody(Response* res, char* body){
	strcat(res->body, "\r\n");
	strcat(res->body, body);
}

// 将结构体中的变量组合成字符串 发送
extern void ResParse(Response* res){
	char final_str[FINAL_STR_SIZE] = {'\0'};
	strcat(final_str, res->Head_code);
	strcat(final_str, res->Server);
	strcat(final_str, res->Date);
	
	strcat(final_str, res->Content_type);
	strcat(final_str, res->Set_cookie);
	strcat(final_str, res->Connection);
	strcat(final_str, res->body);

	ResHandel(res->fd, final_str);
}

// 以纯的字符串返回
extern void Res_row(int acceptFd, char* res_str) {
	char final_str[FINAL_STR_SIZE] = {'\0'};
	strcat( final_str, "HTTP/1.1 200 \r\nContent-type:text/html;utf-8;\r\n\r\n" );
	strcat( final_str, res_str);
	
	ResHandel(acceptFd, final_str);
}


extern void Res_NotFound(int acceptFd){
	char final_str[FINAL_STR_SIZE] = {'\0'};
	strcat( final_str, "HTTP/1.1 404 \r\nContent-type:text/html;utf-8;\r\n\r\n" );
	strcat( final_str, "Not Found");
	
	ResHandel(acceptFd, final_str);
}


extern void Res_render(int acceptFd, char* path, struct Kvmap *kv, int num) {
	char* context = loadTemplate("./templates/test.html");			// 需要释放内存
	char* res = parseContext(context, kv, num-1);						// 模板返回值
	
	Res_row(acceptFd, res);
	free(res);
}
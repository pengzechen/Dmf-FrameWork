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

   
#include <dmfserver/response.h>
#include <dmfserver/utility/utility.h>
#include <dmfserver/socket.h>


// response_t 模块最后调用此函数  发送并关闭此次TCP连接
static void res_handle( connection_tp conn, char* res_str, unsigned int size )
{
	int acceptFd = conn->per_handle_data->Socket;
	int sendbyets = send(acceptFd, res_str, size, 0);
	if (sendbyets == -1) {
		connection_close(conn);
		connection_free(conn);	
	}

	// printf("socket %d: Send: %d byets\n", acceptFd, sendbyets);
	connection_close(conn);
	connection_free(conn);
	// send_next(conn);
}


// 以纯的字符串返回
extern void res_row(connection_tp conn, char* res_str) 
{
	int con_len = strlen(res_str);
	char conlen[8] = {0}; 
	itoa(con_len, conlen, 10); 

	char final_str[FINAL_STR_SIZE] = {0};

	strcat( final_str, "HTTP/1.1 200 OK\r\nContent-type:text/html;utf-8;\r\nConnection: Keep-alive;\r\n" );
	strcat( final_str, "Content-Length: ");strcat( final_str, conlen);strcat( final_str, "\r\n\r\n");
	strcat( final_str, res_str);
	
	res_handle(conn, final_str, strlen(final_str));
}

// 返回 Not Found
extern void res_notfound(connection_tp conn)
{
	// int acceptFd = conn->per_handle_data->Socket;

	char final_str[FINAL_STR_SIZE] = {0};
	strcat( final_str, "HTTP/1.1 404 \r\nContent-type:text/html;utf-8;\r\n"
			"Connection: close;\r\nContent-Length: 18\r\n\r\n" );
	strcat( final_str, "<h1>Not Found</h1>");
	
	res_handle(conn, final_str, strlen(final_str));
}

// 以模板返回
extern void res_render(connection_tp conn, char* template_name, 
						struct Kvmap *kv, int num) 
{
	// int acceptFd = conn->per_handle_data->Socket;

	char* context = get_template(template_name);				// 需要释放内存
	char* res = parse_context(context, kv, num-1);		// 模板返回值  需要释放内存

	res_row(conn, res);
	memset(res, 0, TEMPLATE_RESULT_SIZE);
	free(res);
}


// 中间件调用的返回函数
// *************************************************************************

extern void res_without_permission(connection_tp conn) 
{
	char final_str[FINAL_STR_SIZE] = {0};

	strcat( final_str, "HTTP/1.1 403 \r\n\r\nYou are without permission");
	
	res_handle(conn, final_str, strlen(final_str));
}

// *************************************************************************


// 响应初始化 
// *************************************************************************
// 设置时间和服务器名称
extern void res_init(connection_tp conn, response_t* res)
{
	memset(res->Server, 0, 32);
	memset(res->Content_type, 0, 32);
	memset(res->Date, 0, 32);
	memset(res->Set_cookie, 0, 32);
	memset(res->Connection, 0, 32);
	memset(res->Head_code, 0, 32);
	res->pbody = NULL;

	strcat(res->Server, "Server:DmfServer\r\n");
	
	char time_str[32] = {'\0'};
	server_time(time_str);
	strcat(res->Date, "Date:");
	strcat(res->Date, time_str);
	strcat(res->Date, "\r\n");
	
	strcat(res->Connection, "Connection:keep-alive");
	strcat(res->Connection, "\r\n");
	
	res->conn = conn;
}

// 设置 响应代码（首行）
extern void res_set_head(response_t* res, char* code)
{	
	strcat(res->Head_code, "HTTP/1.1 ");
	strcat(res->Head_code, code);
	strcat(res->Head_code, "\r\n");
}

// 设置 Content-type
extern void res_set_type(response_t* res, char* type)
{
	strcat(res->Content_type, "Content-type:");
	strcat(res->Content_type, type);
	strcat(res->Content_type, "\r\n");
}

// 设置 Set-cookie 
extern void res_set_cookie(response_t* res, char* name, char* value)
{
	strcat(res->Set_cookie, "Set-cookie:");
	strcat(res->Set_cookie, name);
	strcat(res->Set_cookie, "=");
	strcat(res->Set_cookie, value);
	strcat(res->Set_cookie, "\r\n");
}

// 设置 session
extern void res_set_session(response_t*res , char* Session_str) 
{
	
	res_set_cookie(res, "dmfsession", Session_str);
}

// 设置 body
extern void res_set_body(response_t* res, char* body, unsigned int size)
{
	res->pbody = body;
	res->body_size = size;
}

// 将结构体中的变量组合成字符串 发送
extern void res_parse_send(response_t* res) 
{
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

	res_handle(res->conn, final_str, head_len + res->body_size);
	free(final_str);
}



// 以下是静态文件响应函数
// *************************************************************************

extern void res_static(connection_tp conn, char* path, unsigned int size, char* ext, char* content_type) 
{
	// int acceptFd = conn->per_handle_data->Socket;
	if(size > 1024*1024*1) {			//  文件大于 1Mb 调用文件handle
		res_file_handle(conn, path, content_type, size);
		return;
	}
	char* res_str = res_load_file(path);
	response_t res;
	res_init(conn, &res);
	res_set_head(&res, "200");
	res_set_type(&res, content_type);
	res_set_body(&res, res_str, size);
	res_parse_send(&res);
	// free(res.pbody);
	free(res_str);
}

// 返回文件内容指针 调用者使用完文件内容要释放内存
// 对于小文件直接全部读取
static char* res_load_file(char *path) 
{
	FILE *fp;
	fp = fopen( path, "rb" );
	if(fp == NULL){ 
		printf("[response_t: ]%s open Failed\n", path);  
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

// 大文件调用此模块进行返回 
static void res_file_handle(connection_tp conn, char* path, char* content_type, 
							unsigned int size) 
{
	int acceptFd = conn->per_handle_data->Socket;

	char head[512] = {0};
	strcat(head, "HTTP/1.1 200 OK\r\n");
	strcat(head, "Content-Type: ");
	strcat(head, content_type);
	strcat(head, "\r\n");
	strcat(head, "Transfer-Encoding: chunked\r\n\r\n");

	send(acceptFd, head, strlen(head), 0);

	FILE* fp;
	int read_size = 0;
	long long int all_size = 0;
	long long int file_size = 0;
	char buffer[1024*512];
	char chunk_header[10];
	int chunk_header_len = 0;

	fp = fopen(path, "rb");
	if(fp == NULL){
		close_socket(acceptFd);
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
		all_size = all_size + read_size;
	}
	fclose(fp);

	send(acceptFd, "0\r\n\r\n", 5, 0);
	// printf("%d, %ld \n", size, file_size);
	// printf("%d, %ld \n", size, all_size);

	close_socket(acceptFd);
}

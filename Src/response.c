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


// Response 模块最后调用此函数  发送并关闭此次TCP连接
static void ResHandel( int acceptFd, char* res_str, unsigned int size)
{
	int sendbyets = send(acceptFd, res_str, size, 0);

	// printf("[Response: ] Send: %d byets \n", sendbyets);
	#ifdef __WIN32__
	closesocket(acceptFd);
	#elif __linux__
	close(acceptFd);
	#endif // linux 

}


// 以纯的字符串返回
extern void Res_row(int acceptFd, char* res_str) 
{
	char final_str[FINAL_STR_SIZE] = {0};

	strcat( final_str, "HTTP/1.1 200 \r\nContent-type:text/html;utf-8;\r\n\r\n" );
	strcat( final_str, res_str);
	
	ResHandel(acceptFd, final_str, strlen(final_str));
}

// 返回 Not Found
extern void Res_NotFound(int acceptFd)
{
	char final_str[FINAL_STR_SIZE] = {0};
	strcat( final_str, "HTTP/1.1 404 \r\nContent-type:text/html;utf-8;\r\n\r\n" );
	strcat( final_str, "<h1>Not Found</h1>");
	
	ResHandel(acceptFd, final_str, strlen(final_str));
}

// 以模板返回
extern void Res_render(int acceptFd, char* template_name, 
						struct Kvmap *kv, int num) 
{
	char* context = get_template(template_name);				// 需要释放内存
	char* res = parse_context(context, kv, num-1);		// 模板返回值  需要释放内存

	Res_row(acceptFd, res);
	memset(res, 0, TEMPLATE_RESULT_SIZE);
	free(res);
}


// 中间件调用的返回函数
// *************************************************************************

extern void Res_without_permission(int acceptFd) 
{
	char final_str[FINAL_STR_SIZE] = {0};

	strcat( final_str, "HTTP/1.1 403 \r\n\r\nYou are without permission");
	
	ResHandel(acceptFd, final_str, strlen(final_str));
}

// *************************************************************************


// 响应初始化 
// *************************************************************************
// 设置时间和服务器名称
extern void Res_init(int fd, Response* res)
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
	serverTime(time_str);
	strcat(res->Date, "Date:");
	strcat(res->Date, time_str);
	strcat(res->Date, "\r\n");
	
	strcat(res->Connection, "Connection:keep-alive");
	strcat(res->Connection, "\r\n");
	
	res->fd = fd;
}

// 设置 响应代码（首行）
extern void SetHead(Response* res, char* code)
{	
	strcat(res->Head_code, "HTTP/1.1 ");
	strcat(res->Head_code, code);
	strcat(res->Head_code, "\r\n");
}

// 设置 Content-type
extern void SetType(Response* res, char* type)
{
	strcat(res->Content_type, "Content-type:");
	strcat(res->Content_type, type);
	strcat(res->Content_type, "\r\n");
}

// 设置 Set-cookie 
extern void SetCookie(Response* res, char* name, char* value)
{
	strcat(res->Set_cookie, "Set-cookie:");
	strcat(res->Set_cookie, name);
	strcat(res->Set_cookie, "=");
	strcat(res->Set_cookie, value);
	strcat(res->Set_cookie, "\r\n");
}

// 设置 session
extern void SetSession(Response*res , char* Session_str) 
{
	
	SetCookie(res, "dmfsession", Session_str);
}

// 设置 body
extern void SetBody(Response* res, char* body, unsigned int size)
{
	res->pbody = body;
	res->body_size = size;
}

// 将结构体中的变量组合成字符串 发送
extern void ResParseSend(Response* res) 
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

	ResHandel(res->fd, final_str, head_len + res->body_size);
	free(final_str);
}




// 以下是 websocket 响应函数
// *************************************************************************


static void sha1(char sText[], char* shaed) {
	SHA_CTX ctx;
	SHA1_Init(&ctx);

	char temp[10] = {0};
	for (int i = 0; i < strlen(sText); ++i)
	{
		SHA1_Update(&ctx, (void*)&sText[i], 1);
	}
	unsigned char sSHA[20] = {0};
	int ret = SHA1_Final(sSHA, &ctx);
	// printf("ret:%d \n", ret);
	for (int i = 0; i < 20; ++i)
    {
        sprintf(temp, "%02x", sSHA[i]);
		strcat(shaed, temp);
		memset(temp, 0, 10);
    }
}

// 将两个十六进制字符转换为一个字节
static unsigned char hex_to_Byte(const char *hex) {
    unsigned char byte = 0;
    for (int i = 0; i < 2; i++) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            byte = byte * 16 + (hex[i] - '0');
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            byte = byte * 16 + (hex[i] - 'a' + 10);
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            byte = byte * 16 + (hex[i] - 'A' + 10);
        }
    }
    return byte;
}

// 将输入的十六进制字符串转换为字节数组
static unsigned char *hexStringToByteArray(const char *hexString) {
    int len = strlen(hexString);
    if (len % 2 != 0) {
        printf("输入的十六进制字符串长度必须为偶数。\n");
        return NULL;
    }

    int byteArrayLen = len / 2;
    unsigned char *byteArray = (unsigned char *)malloc(byteArrayLen);
    
    for (int i = 0; i < byteArrayLen; i++) {
        byteArray[i] = hex_to_Byte(hexString + i * 2);
    }

    return byteArray;
}

// 发送数据帧
void sendWebSocketFrame(int socket_fd, const char *data, size_t data_length) {
    unsigned char frame[ 1024 ];
    size_t frame_length = 0;

    // 构造帧头
    frame[frame_length++] = 0x81;  // 文本消息标识，最高位为1表示数据帧，次高位为1表示文本消息
    frame[frame_length++] = data_length & 0x7F;  // 数据长度，低7位表示长度

    // 将数据添加到帧中
    for (size_t i = 0; i < data_length; i++) {
        frame[frame_length++] = data[i];
    }

    // 发送帧
    send(socket_fd, frame, frame_length, 0);
}

// 解析WebSocket帧，提取有效数据
int parseWebSocketFrame(const unsigned char *data, size_t data_length, 
						unsigned char **payload, size_t *payload_length) {
    if (data_length < 2) {
        return -1;  // 数据长度不足，无法解析
    }

    // 获取数据帧的掩码标志和有效负载长度
    unsigned char mask = (data[1] >> 7) & 0x01;
    size_t payload_len = data[1] & 0x7F;

    size_t header_length = 2;
    if (payload_len == 126) {
        if (data_length < 4) {
            return -1;  // 数据长度不足，无法解析
        }
        payload_len = (data[2] << 8) | data[3];
        header_length = 4;
    } else if (payload_len == 127) {
        if (data_length < 10) {
            return -1;  // 数据长度不足，无法解析
        }
        // 此处忽略了64位的数据长度，具体情况可以根据实际需要处理
        return -1;  // 数据过长，暂不支持
    }

    size_t total_frame_length = header_length + (mask ? 4 : 0) + payload_len;
    if (data_length < total_frame_length) {
        return -1;  // 数据长度不足，无法解析完整帧
    }

	// 检查关闭帧
	printf("%ld", payload_len);
    if (payload_len == 2 && (data[0] & 0x0F) == 0x08) {
        return -100;  // 连接已断开
    }

    if (mask) {
        unsigned char masking_key[4];
        memcpy(masking_key, data + header_length, 4);

        *payload = (unsigned char *)malloc(payload_len);
        for (size_t i = 0; i < payload_len; i++) {
            (*payload)[i] = data[header_length + 4 + i] ^ masking_key[i % 4];
        }
    } else {
        *payload = (unsigned char *)malloc(payload_len);
        memcpy(*payload, data + header_length, payload_len);
    }

    *payload_length = payload_len;
	

    return 0;  // 成功解析
}

//  将协议转换到websocket
extern void upto_ws_prot(int a, char key[]) 
{
	char sha[128] = {0};
	strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	sha1(key, sha);

	unsigned char *byteArray = hexStringToByteArray(sha);
	char* base64ed = base64_encode(byteArray);
	free(byteArray);
	printf("%s\n%s\n%s\n", key, sha, base64ed);

	char res[4096] = {0};
	int res_len = 0;
	strcat(res, "HTTP/1.1 101 Switching Protocols\r\n");
	strcat(res, "Upgrade: websocket\r\n");
	strcat(res, "Connection: Upgrade\r\n");
	strcat(res, "Sec-WebSocket-Accept: ");
	strcat(res, base64ed);
	strcat(res, "\r\n");
	// strcat(res, "Sec-WebSocket-Protocol: chat\r\n");
	strcat(res, "\r\n");
	res_len = strlen(res);

	int sendbyets = send(a, res, res_len, 0);
	// closesocket(a);

	const char *message = "Hello, WebSocket!";
    sendWebSocketFrame(a, message, strlen(message));
	bool shouldClose = false;

	while(!shouldClose) {
		// 解析数据帧
		unsigned char data[1024];  // 假设接收到的数据

    	size_t data_length = recv( a, data, sizeof(data), 0 );
		if(data_length == 0) {
			close(a);
			return;
		}

		unsigned char *payload;
		size_t payload_length;
		
		int result = parseWebSocketFrame(data, data_length, &payload, &payload_length);
		if (result == 0) {
			printf("解析后的有效数据：");
			for (size_t i = 0; i < payload_length; i++) {
				printf("%c", payload[i]);
			}
			printf("\n");

			free(payload);
		} else if (result == -100) {
			printf("ws closed\n");
			shouldClose = true;  // 设置关闭标志
		}else{
			printf("Can't parse data frame\n");
		}


		const char *message = "Hello, WebSocket!";
		sendWebSocketFrame(a, message, strlen(message));
	}

	close(a);
}



// 以下是静态文件响应函数
// *************************************************************************

extern void Res_static(int acceptFd, char* path, unsigned int size, 
						char* ext, char* content_type) 
{
	if(size > 1024*1024*1) {			//  文件大于 1Mb 调用文件handle
		ResFileHandel(acceptFd, path, content_type, size);
		return;
	}
	char* res_str = loadFile(path);
	Response res;
	Res_init(acceptFd, &res);
	SetHead(&res, "200");
	SetType(&res, content_type);
	SetBody(&res, res_str, size);
	ResParseSend(&res);
	// free(res.pbody);
	free(res_str);
}

// 返回文件内容指针 调用者使用完文件内容要释放内存
// 对于小文件直接全部读取
static char* loadFile(char *path) 
{
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

// 大文件调用此模块进行返回 
static void ResFileHandel(int acceptFd, char* path, char* content_type, 
							unsigned int size) 
{
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
		#ifdef __WIN32__
		closesocket(acceptFd);
		#elif __linux__
		close(acceptFd);
		#endif // linux 
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

	#ifdef __WIN32__
	closesocket(acceptFd);
	#elif __linux__
	close(acceptFd);
	#endif // linux 

}

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

#include <dmfserver/ws.h>

#include <openssl/sha.h>
#include <dmfserver/utility/base64.h>  


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
#ifdef __WIN32__
			closesocket(a);
#else	
			close(a);
#endif // Win32
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
#ifdef __WIN32__
			closesocket(a);
#else	
			close(a);
#endif // Win32
}
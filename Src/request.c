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
#include "request.h"

void MultiParse (Request *request, char *boundary ) {
	
	char *p = NULL;
	char *pp = NULL;
	p = request->body.body;
	pp = p+1;
	
	// 状态参量 这次要不要写字符
	int state = 10;
	int write = 0;
	
	 // 临时记录字符串 
	char temp[1024] = {'\0'};
	int i;
	
	// 记录multidata  
	int k = 0 ;
	
	int name_or_filename = 0;
	
	 // 有几块内容
	int mul_num = -1;
	
	int boundary_len;
	boundary_len = strlen(boundary);
	char b[64];

	
	while(1){
		switch(state){
			
			case 10:
				if(*p == '-' && *pp == '-'){
					state = 11;
					mul_num++;
				}
				break;
			case 11:
				if(*p == '-'){
					write = 0;
				}else{
					write = 1;
				}
				if(*p == '\r' && *pp == '\n'){
					// printf("%s\n", temp);
					state = 90;				 // 校对 boundary
					write = 0;
				}
				if(*p == '-' && *pp == '-'){
					state = 1;
					mul_num--;
				}
				break;
			case 20:
				if(*p == '\n'){

					write = 0;
				}else{
					write = 1;
				}
				if(*p == ':'){
					strcpy(request->multi[mul_num]->dis.key, temp);
					// puts(temp);
					write = 0;
					state = 21;
				}
				break;
			case 21:
				if(*p == ' '){
					write = 0;
				}else{ 
					write = 1; 
				}
				if(*p == ';'){
					strcpy(request->multi[mul_num]->dis.data, temp);
					write = 0;
					state = 22;
				}
				break;
			case 22:
				if(*p == ' '){					// 有空格就空开
					write =  0;
				}else{
					write = 1;
				}
				if(*p == '=' && *pp == '"'){
					
					// puts(temp);
					if(strcmp(temp, "name")==0){
						strcpy(request->multi[mul_num]->name.key, temp);
						name_or_filename = 1;
					}else if(strcmp(temp, "filename")==0){
						strcpy(request->multi[mul_num]->filename.key, temp);
						name_or_filename = 2;
					}
					write = 0;
					state = 235;
				}
				break;
			case 235:							// 跳过 “
				state = 23;
				break;
			case 23:
				write = 1;
				if(*p == '"'){
					 if(name_or_filename == 1){
						strcpy(request->multi[mul_num]->name.data, temp);
					}else if(name_or_filename == 2){
						strcpy(request->multi[mul_num]->filename.data, temp);
					}
					// puts(temp);
					write = 0;
					state = 24;
				}
				break;
			case 24:
				if(*p == ';'){
					state = 22;				// 返回 22
				}
				if(*p == '\r' && *pp == '\n'){
					state = 25;
				}
				break;
			case 25:
				state = 26;     // 跳过 \n
				break;
			case 26:
				state = 27;
				break;
			case 27:
				state = 28;
				break;
			case 28:
				write = 2;
				if(*p == '\r' && *pp == '\n'){
					memset(b, 0, 64);
					memcpy(b, p+4, boundary_len);
					if(strcmp(boundary, b) == 0){
						write = 3;
						state = 10;
					}
				}
				break;
		}
		if(state == 90){
			if(strcmp( temp, boundary ) != 0){
				break;
			} else {
				state = 20;
				request->multi[mul_num] = (struct Multipart *)malloc(sizeof(struct Multipart));
				request->multi[mul_num]->data=(char *)malloc(MULTI_PART_MAX * sizeof(char));	//  创建了内存 记得删除
				memset(request->multi[mul_num]->data, 0, MULTI_PART_MAX);
				// printf("[request] Create a Multi part\n");
			}
		}

		if(write == 2){
			request->multi[mul_num]->data[k] = *p;
			k++;
		}else if(write == 1){
			temp[i] = *p;
			i++;
		}else if(write == 3){
			request->multi[mul_num]->length = k;
			k = 0;
			write = 0;
		}else{
			if(i){
				// k = 0;
				i = 0;
				memset(temp, 0, 1024);
			}
		}
		
		p ++;
		pp = p + 1;
		
		if(state == 1)
			break;
		
		
	}

	request->multi_part_num = mul_num;
	
	
	
	if(MULTI_DEBUG){
		printf("---------------------MULTI-DEBUG--------------------\n");
		for(; mul_num >= 0; mul_num --){
			
		printf("%s  : %s\n", request->multi[mul_num]->dis.key,request->multi[mul_num]->dis.data);
		printf("%s  : %s\n", request->multi[mul_num]->name.key,request->multi[mul_num]->name.data);
		printf("%s  : %s\n", request->multi[mul_num]->filename.key,request->multi[mul_num]->filename.data);
		printf("Length: %d\n", request->multi[mul_num]->length );
		printf("DATA: %#x\n", &(request->multi[mul_num]->data) );
		
		}
		printf("---------------------MULTI-DEBUG--------------------\n");
	}
}


void ParseHttp(Request *request, char *data) {
	
	//p 指向 data 首地址
	char *p;
	char *pp;
	p = data;   
	
	// 解析状态 
	int state = 10;  
	
	int write = 0;
	char temp[512] = {'\0'};
	int i = 0;
	
	//初始化 multi_part_num query param  的个数
	int q_int = 0;
	int p_int = 0;
	request->multi_part_num = -1;		//  -1 表示没有
	
	
	// 计算还有多少 bytes  没有读取
	int num;
	
	
	for(int t=0; t < MULTI_PART_MAX_NUM; t++)
		request->multi[t] = NULL;
	

	
	while(*p != '\0') {
				
		switch(state){
			case 10:
				if( *p == 'G' || *p=='P'){
					write = 1;
					state = 12;
				}else{
					state = -1;			//首字母不是 P G 不合法
				}
				break;
			case 12:
				if( *p == ' '){
					write = 0;
					state = 20;
					strcpy(request->method, temp);
					//printf("%s\n", temp);
				}
				break;
				//处理 path
			case 20:
				if( *p == '/'){
					write = 1;
					state = 21;
				}else{
					state = -2;			//path 不以 / 开头不合法
				}
				break;
			case 21:
				if( *p == '?'){
					write = 0;
					state = 30;
					strcpy(request->path, temp);
					//printf("%s\n", temp);
				}
				if( *p == ' '){
					write = 0;
					state = 50;
					strcpy(request->path, temp);
					//printf("%s\n", temp);
				}
				break;
				//处理 query 
			case 30:    //上一个字符遇到 ？
				write = 1;
				int flag = 1;
				if( *p == '=' ){
					write = 0;
					
					strcpy(request->query[q_int].key, temp);
					//printf("key: %s\n", temp);
					state = 31;
					flag = 0;
				}
				if(*pp == ' ' && flag){
					state = -3;				// 下一个字符出现空格 但还没出现”=“  不合法
				}
				break;
			case 31:
				write = 1;
				if( *p == '&'){
					strcpy(request->query[q_int].data, temp);
					//printf("value: %s\n", temp);
					write = 0;
					state = 30;
					q_int++;
				}
				if (*p == ' '){
					strcpy(request->query[q_int].data, temp);
					//printf("value: %s\n", temp);
					write = 0;
					state = 50;
				}
				break;
			// 解析 协议 版本
			case 50:
				write = 1;
				if(*p == '/'){
					write = 0;
					strcpy(request->protocol, temp);
					//printf("protocol: %s\n", temp);
					state = 51;
				}
				break;
			case 51:
				write = 1;
				if(*p == '\r' && *pp == '\n'){
					write = 0;
					strcpy(request->version, temp);
					//printf("version: %s\n", temp);
					state = 60;
				}
				break;
			//  解析 参数
			case 60:      // 跳过 \n
				state = 61;
				break;
			case 61:
				if(*p >= 'A'&& *p<='Z'){
					write = 1;
				}
					// write = 1;
				if(*p == ':'){
					write = 0;
					strcpy(request->params[p_int].key, temp);
					//printf("key--%d: %s\n", p_int, temp);
					state = 62;
				}
				if(*p == '\r' && *pp == '\n'){		// 进入 body 
				
					p_int--;
					state = 70;
				}
				break;
			
			case 62:    // 跳过 空格
				state = 63;
				break;
			
			case 63:
				write = 1;
				if(*p == '\r' && *pp == '\n'){
					write = 0;
					strcpy(request->params[p_int].data, temp);
					//printf("value--%d: %s\n", p_int, temp);
					
					state = 60;
					
					p_int++;
				}
				break;
			// 解析 body 
			case 70:      // 跳过 \n
				if(strcmp(request->method,"POST") == 0){			
					state = 71;
				}
				else{						// \r\n\r\n  后面没有字符
					state = 1;
					request->body.length = 0;
					request->body.body = NULL;
				}
				break;
			case 71:
				for(int k=p_int;k>=0; k--){
					if( strcmp(request->params[k].key, "Content-Length") == 0){
						int len = atoi(request->params[k].data);
						if( len <= BODY_MAX){
							request->body.length = len;
							request->body.body=(char *)malloc(len + 1);
							memset(request->body.body, 0, len + 1 );
							memcpy(request->body.body, p, len);
						}else{
							printf("too big %d Bytes\n", len);
						}
					}
					break;
				}
				for(int k=p_int;k>=0; k--){
					if( strcmp(request->params[k].key, "Content-Type") == 0) {
						if(strstr(request->params[k].data, "multipart/form-data") != NULL){
							char *b;
							char boundary[64];
							b = strstr(request->params[k].data, "boundary=");
							memset(boundary, 0, 64);
							memcpy(boundary, b+9, strlen(b) - 9);
							MultiParse(request, boundary);
						}
					}
					break;
				}
				state = 1;
				break;
		}
		if(write){
			temp[i] = *p;
			i++;
		}else{
			i=0;
			memset(temp, 0, 512);
		}
		p ++;
		pp = p+1;
		num++;
		if(state == 1)
			break;
	}
	
/*

	switch(state){
		case 1:
			break;
		case -1:
			printf("method invalid\n");
			break;
		case -2:
			printf("path invalid\n");
			break;
		case -3:
			printf("query invalid\n");
			break;
		case -6:
			printf("params invalid\n");
			break;
		default:
			printf("unknow error: %d\n", state);
			break;
	}
*/
	if(state == 10){
		printf("[Server: Info] connection break \n");
	}

	
	request->q_int = q_int;
	request->p_int = p_int;
	
	if(REQUEST_DEBUG){
		
		
		printf("--------------------REQUEST-DEBUG--------------------\n");
		
		printf("----STATE: %d  p_int: %d  q_int: %d----\n", state, p_int, q_int);
		printf("%s\n", request->method);
		printf("%s\n", request->path);
		printf("%s\n", request->protocol);
		printf("%s\n", request->version);
		
		for(;q_int >=0; q_int--){
			printf("%s: %s\n", request->query[q_int].key, request->query[q_int].data);
		}

		for(;p_int >=0; p_int--){
			printf("%s: %s\n", request->params[p_int].key, request->params[p_int].data);
		}
		
		printf("length: %d\nBody: %#x\n", request->body.length, &(request->body.body) );
		printf("--------------------REQUEST-DEBUG--------------------\n");
	}
	
}


void freeReq(Request *req) {
	
	free(req->body.body);
	
	int i=0;
	
	for( ; i <= req->multi_part_num; i++) {
		
		free(req->multi[i]->data);
		
		free(req->multi[i]);
		
	}
}
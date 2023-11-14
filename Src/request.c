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

/*	
	*						REQUEST MODEL
	*
	*	This model parse HTTP reqeust, and save it to "req", which is 
	*	a important struct througout the whole http handle.
	*/

/*	
	*						REQUEST MODEL
	*
	*	This model parse http, and save relevant infomation in a const Request struct
	*	this model doesn't do well, I will imporove it in the future. (Ajax 2023/4/28)
	*   It Reduces server performance by 16 percent.
	*/

#include <dmfserver/request.h>

void req_parse_multi_part (Request *request, char *boundary ) 
{
	
	char *p = NULL;
	p = request->body.body;
	
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
				if(*p == '-' && *(p+1) == '-'){
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
				if(*p == '\r' && *(p+1) == '\n'){
					// printf("%s\n", temp);
					state = 90;				 // 校对 boundary
					write = 0;
				}
				if(*p == '-' && *(p+1) == '-'){
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
				if(*p == '=' && *(p+1) == '"'){
					
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
				if(*p == '\r' && *(p+1) == '\n'){
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
				if(*p == '\r' && *(p+1) == '\n'){
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
		if(state == 1)
			break;
	}

	request->multi_part_num = mul_num;
	
#ifdef MULTI_DEBUG 
	printf("---------------------MULTI-DEBUG--------------------\n");
	for(; mul_num >= 0; mul_num --){
		
	printf("%s  : %s\n", request->multi[mul_num]->dis.key,request->multi[mul_num]->dis.data);
	printf("%s  : %s\n", request->multi[mul_num]->name.key,request->multi[mul_num]->name.data);
	printf("%s  : %s\n", request->multi[mul_num]->filename.key,request->multi[mul_num]->filename.data);
	printf("Length: %d\n", request->multi[mul_num]->length );
	printf("DATA: %#x\n", &(request->multi[mul_num]->data) );
	
	}
	printf("---------------------MULTI-DEBUG--------------------\n");
#endif
}


void req_parse_init (Request *request) {
	// request->pfd = pfd;
	request->query = hashmap_create(17);
	request->params = hashmap_create(17);
}


void req_parse_http(Request *request, char *data )
{
	
	//p 指向 data 首地址
	char *p;
	p = data;   
	
	// 解析状态 
	HTTP_PARSE_STATE state = PARSE_START;  
	
	int write = 0;
	char temp[512] = {'\0'};
	int i = 0;
	
	//初始化 multi_part_num query param  的个数
	request->multi_part_num = -1;		//  -1 表示没有
	
	hashmap_node_t * query_tmp = NULL;
	hashmap_node_t * params_tmp = NULL;

	char *cls = NULL;   //Content-Length
	char *cts = NULL;   //Content-Type
	
	
	for(int t=0; t < MULTI_PART_MAX_NUM; t++)
		request->multi[t] = NULL;
	
	
	
	while( *p != '\0' ) {
				
		switch( state ){
			case PARSE_START:
				if( *p == 'G' || *p=='P'){
					write = 1;
					state = PARSE_METHOD;
				}else{
					state = PARSE_INVALID;			//首字母不是 P G 不合法
				}
				break;
			case PARSE_METHOD:
				if( *p == ' ') {
					write = 0;
					state = PARSE_PATH_START;
					strcpy(request->method, temp);
					//printf("%s\n", temp);
				}
				break;
			
			//处理 path
			case PARSE_PATH_START:
				if( *p == '/'){
					write = 1;
					state = PARSE_PATH;
				}else{
					state = PARSE_INVALID;			//path 不以 / 开头不合法
				}
				break;
			case PARSE_PATH:
				if( *p == '?'){
					write = 0;
					state = PARSE_QUERY_START;
					strcpy(request->path, temp);
					//printf("%s\n", temp);
				}
				if( *p == ' '){
					write = 0;
					state = PARSE_PV_START;
					strcpy(request->path, temp);
					//printf("%s\n", temp);
				}
				break;
			
			//处理 query 
			case PARSE_QUERY_START:    //上一个字符遇到 ？
				write = 1;
				int flag = 1;
				if( *p == '=' ){
					write = 0;
					query_tmp = (hashmap_node_t *) malloc (sizeof(hashmap_node_t));
					query_tmp->next = NULL;
					query_tmp->key = (char*) malloc (sizeof(char)*strlen(temp));
					strcpy(query_tmp->key, temp);
					// printf("key: %s\n", temp);
					state = PARSE_QUERY_TEMP;
					flag = 0;
				}
				if(*(p+1) == ' ' && flag){
					state = PARSE_INVALID;				// 下一个字符出现空格 但还没出现”=“  不合法
				}
				break;
			case PARSE_QUERY_TEMP:
				write = 1;
				if( *p == '&'){
					query_tmp->value = (char*) malloc (sizeof(char)*strlen(temp));
					strcpy(query_tmp->value, temp);
					// printf("value: %s\n", temp);
					hashmap_insert(request->query, query_tmp);
					write = 0;
					state = PARSE_QUERY_START;
				}
				if (*p == ' '){
					query_tmp->value = (char*) malloc (sizeof(char)*strlen(temp));
					strcpy(query_tmp->value, temp);
					// printf("value: %s\n", temp);
					hashmap_insert(request->query, query_tmp);
					write = 0;
					state = PARSE_PV_START;
				}
				break;
			

			// 解析 协议 版本
			case PARSE_PV_START:
				write = 1;
				if(*p == '/'){
					write = 0;
					strcpy(request->protocol, temp);
					//printf("protocol: %s\n", temp);
					state = PARSE_PV_TEMP;
				}
				break;
			case PARSE_PV_TEMP:
				write = 1;
				if(*p == '\r' && *(p+1) == '\n'){
					write = 0;
					strcpy(request->version, temp);
					//printf("version: %s\n", temp);
					state = PARSE_PARAM_START;
				}
				break;
			

			//  解析 参数
			case PARSE_PARAM_START:      // 跳过 \n
				state = PARSE_PARAM1;
				break;
			case PARSE_PARAM1:
				if(*p >= 'A'&& *p<='Z'){
					write = 1;
				}
				if(*p == ':'){
					write = 0;
					params_tmp = (hashmap_node_t *) malloc (sizeof(hashmap_node_t));
					params_tmp->next = NULL;
					params_tmp->key = (char*) malloc (sizeof(char)*strlen(temp));
					strcpy(params_tmp->key, temp);
					// printf("%s\n", temp);
					state = PARSE_PARAM2;
				}
				if(*p == '\r' && *(p+1) == '\n'){		// 进入 body 
					state = PARSE_BODY_START;
				}
				break;
			case PARSE_PARAM2:    // 跳过 空格
				state = PARSE_PARAM3;
				break;
			case PARSE_PARAM3:
				write = 1;
				if(*p == '\r' && *(p+1) == '\n'){
					write = 0;
					params_tmp->value = (char*) malloc (sizeof(char)*strlen(temp));
					strcpy(params_tmp->value, temp);
					hashmap_insert(request->params, params_tmp);
					state = PARSE_PARAM_START;
				}
				break;


			// 解析 body 
			case PARSE_BODY_START:      // 跳过 \n
				if(strcmp(request->method,"POST") == 0){			
					state = PARSE_BODY;
				}
				else{						// \r\n\r\n  后面没有字符
					state = 1;
					request->body.length = 0;
					request->body.body = NULL;
				}
				break;
			case PARSE_BODY:
				
				cls = (char*)hashmap_get(request->params, "Content-Length");
				if ( cls != NULL) {
					int len = atoi(cls);
					if( len <= HTTP_BODY_MAX) {
						request->body.length = len;
						request->body.body=(char *)malloc(len + 1);
						memset(request->body.body, 0, len + 1 );
						memcpy(request->body.body, p, len);
					} else {
						printf("too big %d Bytes\n", len);
					}
				}
				
				cts = hashmap_get(request->params, "Content-Type");
				if( cts != NULL) {
					if(strstr(cts, "multipart/form-data") != NULL){
						char *b;
						char boundary[64];
						b = strstr(cts, "boundary=");
						memset(boundary, 0, 64);
						memcpy(boundary, b+9, strlen(b) - 9);
						req_parse_multi_part(request, boundary);
					}
				}
					
				state = 1;
				break;
		}
		
		if (write) {
			temp[i] = *p;
			i++;
		} else {
			i=0;
			memset(temp, 0, 512);
		}
		p ++;

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

	
#ifdef REQUEST_DEBUG 
	printf("--------------------REQUEST-DEBUG--------------------\n");
	printf("----STATE: %d  p_int: %d  q_int: %d----\n", state, p_int, q_int);
	printf("%s\n", request->method);
	printf("%s\n", request->path);
	printf("%s\n", request->protocol);
	printf("%s\n", request->version);
	printf("length: %d\nBody: %#x\n", request->body.length, &(request->body.body) );
	printf("--------------------REQUEST-DEBUG--------------------\n");
#endif
	
}


void req_get_session_str(const Request* req, char session_str[]) // OUT 
{
    char* temp;
	char* data = hashmap_get(req->params, "Cookie");

	if( data != NULL ) {
		temp = strstr(data, "dmfsession=");
		if( temp != NULL && strlen(temp) >= 21 ){  //
			*(temp + 21) = '\0';
			strcpy(session_str, temp + 11);
		}
	}

}


void req_get_ws_key(const Request* req, char ws_key[]) 			// OUT 
{
	char* data = hashmap_get(req->params, "Sec-WebSocket-Key");
	if( data != NULL ) {
		strcpy(ws_key, data);
	}

}


void req_get_param(const Request *req, char* key, char data[]) // OUT
{
	char* data1 = hashmap_get(req->params, key);
	if( data1 != NULL ) {
		strcpy(data, data1);
	}

}


void req_get_query(const Request *req, char* key, char data[]) // OUT
{
	char* data1 = hashmap_get(req->query, key);
	if( data1 != NULL ) {
		strcpy(data, data1);
	}

}


void req_free(Request *req) 
{

	hashmap_destroy(req->query);
	hashmap_destroy(req->params);
	
	free(req->body.body);
	
	int i=0;
	
	for( ; i <= req->multi_part_num; i++) {
		
		free(req->multi[i]->data);
		
		free(req->multi[i]);
		
	}
}
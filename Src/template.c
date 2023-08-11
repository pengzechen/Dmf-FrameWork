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


#include <dmfserver/template.h>

static template_dec g_template_dec;

// load all template to memery
void template_init() 
{
	g_template_dec.size = 2;
	strcpy(g_template_dec.template_np[0].name, "test.html");
	strcpy(g_template_dec.template_np[1].name, "test2.html");
	g_template_dec.template_np[0].template_data = local_template("./templates/test.html");
	g_template_dec.template_np[1].template_data = local_template("./templates/test2.html");
	printf("[Template Info] template init ok!\n");
	printf("\n");
}


char* local_template(char * template_path)
{
	FILE *fp;
	fp = fopen( template_path, "r" );
	if(fp == NULL) { 
		printf("[Template: ]open Failed \n");
		char* res = (char*)malloc(sizeof(char)*1);
		strcpy(res, ""); 
		return res; 
	}
	unsigned long int file_size;
	fseek(fp, 0L, 2);
	file_size = ftell(fp);
	fseek(fp, 0L, 0);
	
	char * _context = (char*)malloc(file_size * sizeof(char) + 1);  // 模板文件一般不会太大， 直接全部读取
	memset(_context, '\0', file_size * sizeof(char) + 1);

	fread(_context, sizeof(char), file_size, fp);
	fclose(fp);
	
	return _context;
}


char* get_template(char* template_name) 
{
	for(int i=0; i< g_template_dec.size; i++) {
		if( strcmp(template_name, g_template_dec.template_np[i].name) == 0 )
			return g_template_dec.template_np[i].template_data; 
	}
	return "";
}


void parse_dec(char* tt, char* dec[], char* inner) 
{
	char other[1024] = {'\0'};
	char *start = strstr(inner, "{{item}}");
	*start = '\0';
	strcpy(other, start + 8);
		
	for(int i=0; i<=20; i++){
		if(dec[i] == NULL)
			break;
		strcat(tt, inner);
		strcat(tt, dec[i]);
		strcat(tt, other);
	}
}


char* parse_context(char *context, struct Kvmap *kv, int kv_num)
{
	
	char *p = context;
	char *pp = p + 1;
	
	int write_temp = 0;
	char temp[1024] = {'\0'};
	int temp_pos = 0;
	
	int write_res = 1;				// 当前是否要写
	int res_pos = 0;				// 最终解析的长度
	
	int state = 0;

	char block_str[1024] = {'\0'};	 // [@xxx@' '@] '' 之间的字符串

	char parsed_str[1024] = {'\0'};  // parse dec 或者是 回调函数的返回值
	
	char *result = (char*)malloc(sizeof(char) * TEMPLATE_RESULT_SIZE );
	memset(result, '\0', TEMPLATE_RESULT_SIZE );
	
	
	
	while(*p != '\0'){
		
		switch(state){
			
			case 0:									// 字符查找 满足标识则跳转
				write_res = 1;
			
				if(*p == '[' && *pp == '#'){
					write_res = 0;
					state = -1;
				}
				if(*p == '[' && *pp == '@'){
					write_res = 0;
					state = -11;
				}
				break;		// 正常字符不做处理
			
			case -1:					// 跳过 #
				state = 1;
				break;
			
			case -11:					// 跳过 #
				state = 2;
				break;
			
			case 1:							
				write_temp = 1;
				if(*p == '#' && *pp == ']') {
					write_temp = 0;
					// printf("%s", temp); 
					for(int num=0; num<= kv_num; num++) {
						if( strcmp(kv[num].key, temp) == 0 && kv[num].type == 1){
							// printf("%s   ", kv[num].value);
							strcat(result, kv[num].value);
							res_pos = res_pos + strlen(kv[num].value) ;
							// printf("%s", result);
						}
					}
					state = -2;
				}
				break;
				
			case 2:							
				write_temp = 1;
				if(*p == '@' && *pp == ']') {
					write_temp = 0;
					char* start = strstr(temp, "@");

					strcpy( block_str, start + 1 );
					*start = '\0';
					
					for(int num=0; num<= kv_num; num++) {
						if( strcmp(kv[num].key, temp) == 0 && kv[num].type == 2) {
							
							kv[num].Func(parsed_str, block_str);
							
							strcat(result, parsed_str);
							res_pos = res_pos + strlen(parsed_str);
							memset(block_str, 0, 1024);
						}
						if( strcmp(kv[num].key, temp) == 0 && kv[num].type == 3) {
							
							parse_dec(parsed_str, kv[num].dec, block_str);
							
							strcat(result, parsed_str);
							res_pos = res_pos + strlen(parsed_str);
							memset(block_str, 0, 1024);
						}
					}
					state = -2;
				}
				break;
			
			case -2:					// 跳过  ] ]
				state = 0;
				break;
			
			default:
				printf(" Not Catch");
				break;
		}
		
		if(write_temp) {
			temp[temp_pos] = *p;
			temp_pos ++ ;
		} else {
			if(temp_pos) { temp_pos = 0; memset( temp, '\0', 64); }
		}
		
		// 普通字符串应该要写到 result 中
		if(write_res) {
			result[res_pos] = *p;
			res_pos ++;
		}
		// 向后迭代
		p++;
		pp = p + 1;
	}
	res_pos ++;
	result[res_pos] = '\0';
			
	// printf("res_pos: %s\n", result);
	return result;
}


void template_free() 
{
	for(int i=0; i<=g_template_dec.size; i++) {
		free(g_template_dec.template_np[i].template_data);
	}
}
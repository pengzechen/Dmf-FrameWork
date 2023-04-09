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


#include <dmfserver/template.h>

char * loadTemplate(char * template_path) {
	FILE *fp;
	fp = fopen( template_path, "r" );
	if(fp == NULL){ printf("[Template: ]open Failed \n"); return ""; }
	unsigned long int file_size;
	fseek(fp, 0L, 2);
	file_size = ftell(fp);
	fseek(fp, 0L, 0);
	
	char * _context = (char*)malloc(file_size * sizeof(char));  // 模板文件一般不会太大， 直接全部读取
	memset(_context, '\0', file_size * sizeof(char));
	fread(_context, sizeof(char), file_size, fp);
	fclose(fp);
	
	return _context;
}


void ParseDec(char* tt, char* dec[], char* inner) {
	
	char *start = strstr(inner, "{{item}}");
	char other[1024] = {'\0'};
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

// 释放存放模板信息字符串  context
char* parseContext(char *context, struct Kvmap *kv, int kv_num) {
	
	char *p = context;
	char *pp = p + 1;
	
	int write_temp = 0;
	char temp[1024] = {'\0'};
	int i;
	
	int write_res = 1;
	int k=0;
	
	int state = 0;
	
	char *result = (char*)malloc(sizeof(char) * TEMPLATE_RESULT_SIZE );
	memset(result, 0, TEMPLATE_RESULT_SIZE );
	
	
	
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
						if(kv[num].type == 1){
							if( strcmp(kv[num].key, temp) == 0 ){
								// printf("%s   ", kv[num].value);
								strcat(result, kv[num].value);
								k = k + strlen(kv[num].value) ;
								// printf("%s", result);
							}
						}
					}
					
					state = -2;
				}
				break;
				
			case 2:							
				write_temp = 1;
				if(*p == '@' && *pp == ']'){
					write_temp = 0;
					char* start = strstr(temp, "@");

					char str[1024] = {'\0'};
					strcpy( str, start+1 );
					*start = '\0';
					
					for(int num=0; num<= kv_num; num++) {
						if(kv[num].type == 2){
							if( strcmp(kv[num].key, temp) == 0 ){
								char tt[1024] = {'\0'};
								kv[num].Func(tt, str);								
								strcat(result, tt);
								k = k + strlen(tt) ;
							}
						}
						if(kv[num].type == 3){
							if( strcmp(kv[num].key, temp) == 0 ){
								char tt[1024] = {'\0'};
								ParseDec(tt, kv[num].dec, str);
								strcat(result, tt);
								k = k + strlen(tt) ;
							}
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
		

		if(write_temp){
			temp[i] = *p;
			i ++ ;
		}else{
			if(i){
				i = 0;
				memset( temp, '\0', 64);
			}
		}
		
		if(write_res){
			result[k] = *p;
			k++;
		}
		
		p++;
		pp = p + 1;
	}
	
	free(context);			// 释放存放模板信息字符串
	return result;
}


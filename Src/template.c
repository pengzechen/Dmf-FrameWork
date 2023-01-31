
#include "template.h"

char * loadTemplate(char * template_path) {
	FILE *fp;
	fp = fopen( template_path, "r" );
	if(fp == NULL){ printf(" open Failed");  }
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


void function(char *out, char *in) {
	// 
	strcpy(out, in);
}


void usage() {
	
	struct Kvmap kv[4];
	
	kv[0].key = "name";
	kv[0].value = "pzcbnvvhjv";
	kv[0].type = 1;
	
	kv[1].key = "age";
	kv[1].value = "22ghfifuif";
	kv[1].type = 1;

	kv[2].key = "myfunction";
	kv[2].Func = &function;
	kv[2].type = 2;
	
	kv[3].key = "dec1";
	kv[3].dec[0] = "one";
	kv[3].dec[1] = "two";
	kv[3].dec[2] = "three";
	kv[3].dec[3] = NULL;
	kv[3].type = 3;
	
	char* context = loadTemplate("./templates/test.html");			// 需要释放内存
	char* res = parseContext(context, kv, 4-1);
	
	printf("%s", res);
	
	free(context);
	free(res);
	
}
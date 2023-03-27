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
#include "router.h"

// 全局 view 回调函数
ContFunMap g_cmp;

void Rou_init(
	// 服务启动时传来的
	ContFun cf[], 
	char* keys[], 
	int acceptFd, 
	Request *req) 
{
	int flag = 0;
	
	// 先在control回调函数列表中寻找
	for(int i=0; cf[i] != NULL; i++){
		if( strcmp(req->path, keys[i]) == 0){
			ContFun p;
			p = cf[i];
			p(acceptFd, req);
			flag = 1;	// 回调函数找到了
		}
	}
	if(flag == 0) {
	
		char* local_path[ STATIC_FILES_MAX_NUM ] = {NULL};
		
		int num = searchLocalFile(local_path);
		for(int i=0; i <= num; i++){

			if( strcmp(req->path, local_path[i]) == 0){
				
				char name[512] = {'\0'};
				strcat(name, STATIC_DIR);
				strcat(name, local_path[i]);
				char* block = loadFile(name);
				Res_row(acceptFd, block);
				free(block);
				flag = 1;	// 静态资源找到了
			}
		}
		for(int i=0; i <= num; i++){
			free(local_path[i]);
		}
		
		if(flag == 0){
			
			Res_NotFound(acceptFd);
		}
	}
}

void Rou_iocp_init(
	ContFunMap cmp, 
	int acceptFd, 
	Request *req)
{
	int flag = 0;
	
	// 先在control回调函数列表中寻找
	for(int i=0; cmp.keys[i] != NULL; i++) {
		if( strcmp(req->path, cmp.keys[i]) == 0){
			ContFun p;
			p = cmp.cf[i];
			p(acceptFd, req);
			flag = 1;	// 回调函数找到了
		}
	}
	
	if(flag == 0) {
	
		char* local_path[ STATIC_FILES_MAX_NUM ] = {NULL};
		
		int num = searchLocalFile(local_path);
		for(int i=0; i <= num; i++){

			if( strcmp(req->path, local_path[i]) == 0){
				
				char name[1024] = {'\0'};
				strcat(name, STATIC_DIR);
				strcat(name, local_path[i]);
				char* block = loadFile(name);
				Res_row(acceptFd, block);
				free(block);
				flag = 1;	// 静态资源找到了
			}
		}
		for(int i=0; i <= num; i++){
			free(local_path[i]);
		}
		
		if(flag == 0){
			
			Res_NotFound(acceptFd);   // 返回404Not Found
		}
	}
}

int searchLocalFile(char* local_paths[]){
	struct dirent *ptr;
	DIR *dir;
	
	int file_nums = -1;
	
	dir=opendir(STATIC_DIR);
	while((ptr=readdir(dir))!=NULL) {
		if(ptr->d_name[0] == '.')
			continue;
		
		file_nums++;
		
		local_paths[file_nums] = (char*)malloc(sizeof(char)*512);
		memset(local_paths[file_nums], 0, sizeof(char)*512);
		strcat(local_paths[file_nums], "/");
		strcat(local_paths[file_nums], ptr->d_name); 
		
	}
	closedir(dir);
	
	return file_nums;
}


char* loadFile(char *path) {
	FILE *fp;
	fp = fopen( path, "r" );
	if(fp == NULL){ printf(" open Failed");  }
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


void Router_init() {
	for(int i=0; i < ContFunNUM; i++){
		g_cmp.cf[i] = NULL;
		g_cmp.keys[i] = NULL;
	}
	g_cmp.curr_num = 0;

	printf("[Router: Info] Router init successfully...\n");
}

#define RED "\033[0;32;31m"
#define NONE "\033[m"
#define YELLOW "\033[1;33m"

void router_add_app(ContFun cf[], char* keys[], const char* name) {
	
	int icf=0, ikeys=0;
	while(cf[icf] != NULL){
		icf ++;
		if(icf > 1000) return;
	}
	while(keys[ikeys] != NULL){
		ikeys ++;
		if(ikeys > 1000) return;
	}
	char appname[64] = {0};

	if(icf != ikeys)
	{
		printf("[Router: info] App: %s Failed \n", name);
	}
	int curr_num = g_cmp.curr_num;

	for(int i = 0; i < icf; i++){
		g_cmp.cf[ curr_num + i ] = cf[i];
		strcat(appname, "/");
		strcat(appname, name);
		strcat(appname, keys[i]);
		g_cmp.keys[ curr_num + i ] = (char*)malloc(sizeof(char) * strlen(appname));
		strcpy(g_cmp.keys[ curr_num + i ], appname);
		memset(appname, 0, 64);
	}

	g_cmp.curr_num = g_cmp.curr_num + icf;

	printf("[Router: info] App: "YELLOW" %s"NONE" %d function loaded\n", name, icf);
}
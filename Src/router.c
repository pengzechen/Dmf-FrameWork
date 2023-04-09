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
#include <dmfserver/router.h>

// 全局 view 回调函数
ContFunMap g_cmp;
struct FileInfo g_file_list[MAX_FILES];
int g_num_files = 0;

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
	
		for(int i=0; i <= g_num_files; i++){

			if( strcmp(req->path, g_file_list[i].url) == 0){
				
				Res_static(acceptFd, g_file_list[i].path, g_file_list[i].size, g_file_list[i].ext, g_file_list[i].content_type);

				flag = 1;	// 静态资源找到了
			}
		}
		
		if(flag == 0){
			
			Res_NotFound(acceptFd);
		}
	}
}


void Rou_iocp_handle(ContFunMap cmp, int acceptFd, Request *req) {
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
	
		// char* local_path[ STATIC_FILES_MAX_NUM ] = {NULL};
		// int num = searchLocalFile(local_path);
		for(int i=0; i <= g_num_files; i++){
			if( strcmp(req->path, g_file_list[i].url) == 0){
				Res_static(acceptFd, g_file_list[i].path, g_file_list[i].size, g_file_list[i].ext, g_file_list[i].content_type);
				flag = 1;	// 静态资源找到了
			}
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


void traverse_directory(const char *path, struct FileInfo file_list[], int *num_files) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL) {
        printf("Error: Failed to open directory %s\n", path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, MAX_PATH_LENGTH, "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) {
            printf("Error: Failed to get file information for %s\n", full_path);
            continue;
        }

        struct FileInfo info;
		memset(&info, 0, sizeof(struct FileInfo));
        strncpy(info.path, full_path, MAX_PATH_LENGTH);
		
		
        if (entry->d_type == DT_DIR) {
            strncpy(info.type, "Directory", 16);
            info.size = 0;
            info.ext[0] = '\0';
            traverse_directory(full_path, file_list, num_files);
        } else {
            strncpy(info.type, "File", 16);
            info.size = st.st_size;
            char *dot_pos = strrchr(entry->d_name, '.');
            if (dot_pos == NULL || dot_pos == entry->d_name) {
                info.ext[0] = '\0';
            } else {
                strncpy(info.ext, dot_pos + 1, 16);
            }
        }

        if (*num_files >= MAX_FILES) {
            printf("Warning: Reached maximum number of files.\n");
            break;
        }

        file_list[*num_files] = info;
        (*num_files)++;
    }

    closedir(dir);
}


void Router_init() {
	for(int i=0; i < ContFunNUM; i++){
		g_cmp.cf[i] = NULL;
		g_cmp.keys[i] = NULL;
	}
	g_cmp.curr_num = 0;
	char buffer[1024];

    if( !getcwd(buffer, 1024) ) {
        perror("getcwd error\n");  
		return;
    }  
	char static_dir[1024] = {0};
	
	strcat(static_dir, buffer);
	strcat(static_dir, "/");

	printf("%s\n", static_dir);
	strcat(static_dir, g_server_conf_all._conf_router.static_dir);
	//free(buffer);

	traverse_directory(static_dir, g_file_list, &g_num_files);

	#ifdef Router_Debug
    printf("Found %d files.\n", g_num_files);
	#endif
	char* content_type;
	char* url;
    for (int i = 0; i < g_num_files; i++) {
		content_type = get_content_type(g_file_list[i].ext);
		strcpy(g_file_list[i].content_type, content_type);

		url = strstr(g_file_list[i].path, "/");
		strcpy(g_file_list[i].url, url);
        #ifdef Router_Debug
		printf("%d: %s \n    (%s, "YELLOW"%ld bytes"NONE", %s, %s, "YELLOW"%s"NONE")\n",
				i, g_file_list[i].path, 
				g_file_list[i].type, g_file_list[i].size, 
				g_file_list[i].ext, g_file_list[i].content_type, g_file_list[i].url);

        // printf(" (%s, %ld bytes, %s, %s)\n", g_file_list[i].type, g_file_list[i].size, g_file_list[i].ext, g_file_list[i].content_type);
		#endif
	}
	

	printf("[Router: Info] Router init successfully...\n");
}


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


char* get_content_type(char *file_ext) {
    int i;
    char *ext = malloc(strlen(file_ext) + 1);
    strcpy(ext, file_ext);
    for (i = 0; ext[i]; i++) {
        ext[i] = tolower(ext[i]);
    }
    if (strcasecmp(ext, "html") == 0 || strcasecmp(ext, "htm") == 0) {
		free(ext);
        return "text/html";
    } else if (strcasecmp(ext, "txt") == 0) {
		free(ext);
        return "text/plain";
    } else if (strcasecmp(ext, "css") == 0) {
		free(ext);
        return "text/css";
    } else if (strcasecmp(ext, "js") == 0) {
		free(ext);
        return "application/javascript";
    } else if (strcasecmp(ext, "json") == 0) {
		free(ext);
        return "application/json";
    } else if (strcasecmp(ext, "xml") == 0) {
		free(ext);
        return "application/xml";
    } else if (strcasecmp(ext, "gif") == 0) {
		free(ext);
        return "image/gif";
    } else if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0) {
		free(ext);
        return "image/jpeg";
    } else if (strcasecmp(ext, "png") == 0) {
		free(ext);
        return "image/png";
    } else if (strcasecmp(ext, "bmp") == 0) {
		free(ext);
        return "image/bmp";
    } else if (strcasecmp(ext, "ico") == 0) {
		free(ext);
        return "image/x-icon";
    } else if (strcasecmp(ext, "pdf") == 0) {
		free(ext);
        return "application/pdf";
    } else if (strcasecmp(ext, "doc") == 0 || strcasecmp(ext, "docx") == 0) {
		free(ext);
        return "application/msword";
    } else if (strcasecmp(ext, "xls") == 0 || strcasecmp(ext, "xlsx") == 0) {
		free(ext);
        return "application/vnd.ms-excel";
    } else if (strcasecmp(ext, "ppt") == 0 || strcasecmp(ext, "pptx") == 0) {
		free(ext);
        return "application/vnd.ms-powerpoint";
    }else if (strcasecmp(ext, "mp4") == 0) {
		free(ext);
        return "audio/mp4";
    } else {
		free(ext);
        return "application/octet-stream";
    }
    
}
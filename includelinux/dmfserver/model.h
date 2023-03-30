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
#ifndef MODEL
#define MODEL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* 引入连接Mysql的头文件和lib包 */
#include <mysql/mysql.h>
// #pragma comment(lib,"libmysql.lib")

/*定义一些数据库连接需要的宏*/
#define HOST "localhost" 			/*MySql服务器地址*/
#define USERNAME "root" 			/*用户名*/
#define PASSWORD "dmfserver2023" 	/*数据库连接密码*/
#define DATABASE "test" 			/*需要连接的数据库*/

typedef struct _Model {
	MYSQL* my_connection;
	MYSQL_RES* res_ptr;
	int row;
	int column;
	
} Model;

typedef Model* ModelPtr;


#ifdef __cplusplus
extern "C" {
#endif

//  mysql_affected_rows(&my_connection)  返回int

extern void exeSql(char* sql);

void ModelInit(ModelPtr mPtr);

int ModelConnection(ModelPtr mPtr);

int ModelQuery(ModelPtr mPtr, char* sql);

static void GetModelResultInfo(ModelPtr mPtr);

int GetModelResult(ModelPtr mPtr);

void ModelClose(ModelPtr mPtr);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif
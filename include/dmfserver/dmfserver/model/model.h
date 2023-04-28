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


#ifndef __MODEL_INCLUDE__
#define __MODEL_INCLUDE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <dmfserver/conf/conf.h>


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

	extern void ModelInit(ModelPtr mPtr);

	extern int ModelConnection(ModelPtr mPtr);

	extern int ModelQuery(ModelPtr mPtr, char* sql);

	static void GetModelResultInfo(ModelPtr mPtr);

	extern int GetModelResult(ModelPtr mPtr);

	extern void ModelClose(ModelPtr mPtr);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __MODEL_INCLUDE__
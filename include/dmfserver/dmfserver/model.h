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
#include <dmfserver/cpool.h>


typedef struct _Model {
    mysql_conn      *       cc;
	MYSQL_RES       *       res_ptr;
	int                     row;
	int                     column;
} Model;

typedef Model* model_ptr_t;


#ifdef __cplusplus
extern "C" {
#endif

//  mysql_affected_rows(&my_connection)  返回int

extern void exe_sql(char* sql);

extern void model_init(model_ptr_t mPtr);

extern int model_connection(model_ptr_t mPtr);

extern int model_query(model_ptr_t mPtr, char* sql);

static void get_model_result_info(model_ptr_t mPtr);

extern int get_model_result(model_ptr_t mPtr);

extern void model_close(model_ptr_t mPtr);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __MODEL_INCLUDE__
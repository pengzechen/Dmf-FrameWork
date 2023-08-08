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
#include <dmfserver/model.h>


void model_init(model_ptr_t mPtr) 
{
	mPtr->cc = get_mysql_connection_block();
	mPtr->res_ptr = NULL;
}


// 返回值为0执行成功
int model_query(model_ptr_t mPtr, char* sql) 
{
	return mysql_query(&mPtr->cc->conn, sql);
}


static void get_model_result_info(model_ptr_t mPtr) 
{
	mPtr->row = mysql_num_rows(mPtr->res_ptr);
	mPtr->column = mysql_num_fields(mPtr->res_ptr);
}


// 保存查询到的信息 行数 列数
int get_model_result(model_ptr_t mPtr) 
{
	mPtr->res_ptr = mysql_store_result(&mPtr->cc->conn);
	if(mPtr->res_ptr){
		get_model_result_info(mPtr);
		return 1;
	}else{
		return 0;
	}
}


void exe_sql(char* sql) 
{

	Model modeltest;
	model_ptr_t mPtr= &modeltest;
	model_init(mPtr);

	model_query(mPtr, "set names utf8");

	if ( model_query(mPtr, sql) ) {
		printf("Query Error \n");
		return;
	} 
	

    MYSQL_ROW result_row;	/* 按行返回查询信息 */
	// 存在则输出
	if (get_model_result(mPtr)) {

		for (int i = 0; i < mPtr->row; i++) {
			// 一行数据
			result_row = mysql_fetch_row(mPtr->res_ptr);
			
			for (int j = 0; j < mPtr->column; j++) {
				printf( "%s ", result_row[j] );
			}
			
			printf("\n");
		}
	}
	mysql_free_result(mPtr->res_ptr);
	free(mPtr->res_ptr);

	release_mysql_connection(mPtr->cc);
}


// 返回值非零 连接成功
// 不启用连接池时，使用此函数连接MySQL
int model_connection(model_ptr_t mPtr) 
{
	if( !mysql_real_connect(&mPtr->cc->conn, 
						g_server_conf_all._conf_model.host, 
						g_server_conf_all._conf_model.username, 
						g_server_conf_all._conf_model.password, 
						g_server_conf_all._conf_model.database, 
						0, NULL, CLIENT_FOUND_ROWS))
	{
		free(&mPtr->cc->conn);
		printf("connection failed! \n");
		return 0;
	}
}

// 注意，此函数必须在不启用连接池时使用
void model_close(model_ptr_t mPtr) 
{
	mysql_close(&mPtr->cc->conn);
	free(&mPtr->cc->conn);
}

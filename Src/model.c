
#include "model.h"

void ModelInit(ModelPtr mPtr){
	mPtr->my_connection = (MYSQL*)malloc(sizeof(MYSQL));
	mPtr->res_ptr = NULL;
	mysql_init(mPtr->my_connection);
}





// 返回值非零 连接成功
int ModelConnection(ModelPtr mPtr){
	if(mysql_real_connect(mPtr->my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0,NULL, CLIENT_FOUND_ROWS))
	{
		return 1;
	}else{
		free(mPtr->my_connection);
		return 0;
	}
}





// 返回值为0执行成功
int ModelQuery(ModelPtr mPtr, char* sql){
	return mysql_query(mPtr->my_connection, sql);
}





static void GetModelResultInfo(ModelPtr mPtr){
	mPtr->row = mysql_num_rows(mPtr->res_ptr);
	mPtr->column = mysql_num_fields(mPtr->res_ptr);
}
// 保存查询到的信息 行数 列数
int GetModelResult(ModelPtr mPtr){
	mPtr->res_ptr = mysql_store_result(mPtr->my_connection);
	if(mPtr->res_ptr){
		GetModelResultInfo(mPtr);
		return 1;
	}else{
		return 0;
	}
}





void ModelClose(ModelPtr mPtr){
	mysql_close(mPtr->my_connection);
	free(mPtr->my_connection);
}





void exeSql(char* sql) {

	Model modeltest;
	ModelPtr mPtr= &modeltest;
	ModelInit(mPtr);
	
	if( !ModelConnection(mPtr) ) {
		printf("Connection Error \n");
		return;
	}

	ModelQuery(mPtr, "set names utf8");

	if ( ModelQuery(mPtr, sql) ) {
		printf("Query Error \n");
		ModelClose(mPtr);
		return;
	} 
	

    MYSQL_ROW result_row; 	/*按行返回查询信息*/ 
	// 存在则输出
	if (GetModelResult(mPtr)) {

		for (int i = 0; i < mPtr->row; i++) {
			// 一行数据
			result_row = mysql_fetch_row(mPtr->res_ptr);
			
			for (int j = 0; j < mPtr->column; j++) {
				printf( "%s ", result_row[j] );
			}
			
			printf("\n");
		}
	}


	ModelClose(mPtr);

}
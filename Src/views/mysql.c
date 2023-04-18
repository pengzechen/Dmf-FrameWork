#include <dmfserver/model/model.h>


void mysqltest(int a, const Request* req)
{
	
	mysql_conn* conn1 = get_mysql_connection_block();
	mysql_query(&conn1->conn, "select * from test;");
	MYSQL_RES* res_ptr = mysql_store_result(&conn1->conn);
	//printf("   query result %d\n", mysql_num_rows(res_ptr));
	mysql_free_result(res_ptr);
	free(res_ptr);

	Res_row(a, "ok");
	release_mysql_connection(conn1);
}

void mysqltest1(int a, const Request* req) 
{
	
	exeSql("select * from test;");

	Res_row(a, "ok");
}

RouterAdd(model)
{
	ContFun cf[] = {&mysqltest, &mysqltest1,NULL};
	char* keys[] = {"/mysqltest", "/mysqltest1",NULL};
	router_add_app(cf, keys, __func__);
}
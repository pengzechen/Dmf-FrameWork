#include <dmfserver/mdb/mdb.h>
#include <dmfserver/request.h>


void mdbtest(int a, const Request* req)
{
	char* str_from_mdb;
	str_from_mdb = mdb_find("123");
	Res_row(a, str_from_mdb);
	free(str_from_mdb);
}


RouterAdd(mdb){
	ContFun cf[] = {&mdbtest, NULL};
	char* keys[] = {"/test", NULL};
	router_add_app(cf, keys, __func__);
}


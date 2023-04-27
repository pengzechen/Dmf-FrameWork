#include <dmfserver/mdb/mdb.h>
#include <dmfserver/request.h>


void mdbtest(int a, const Request* req)
{
	Res_row(a, mdb_find("123"));
}


RouterAdd(mdb){
	ContFun cf[] = {&mdbtest, NULL};
	char* keys[] = {"/test", NULL};
	router_add_app(cf, keys, __func__);
}


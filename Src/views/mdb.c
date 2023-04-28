#include <dmfserver/mdb/mdb_operate.h>
#include <dmfserver/request.h>


void mdbfind(int a, const Request* req)
{
	char* str_from_mdb;
	
	char data[40] = {0};
	
	strcpy(data, req->query[0].data);
	char* pdata = data;

	str_from_mdb = mdb_find(pdata);
	Res_row(a, str_from_mdb);
	free(str_from_mdb);
}

void mdbinsert(int a, const Request* req)
{
	char ckey[64] = {0};
	char cdata[64] = {0};
	strcpy(ckey, req->query[0].key);
	strcpy(cdata, req->query[0].data);
	char* key = ckey;
	char* data = cdata;
	mdb_insert(key, data);
	Res_row(a, "insert ok!");
}


RouterAdd(mdb){
	ContFun cf[] = {&mdbfind, &mdbinsert, NULL};
	char* keys[] = {"/find", "/insert", NULL};
	router_add_app(cf, keys, __func__);
}


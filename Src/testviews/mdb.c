#include <dmfserver/mdb/mdb_operate.h>
#include <dmfserver/request.h>


void mdbfind(int a, const Request* req)
{
	char* str_from_mdb;
	
	char data[40] = {0};
	
	strcpy(data, hashmap_get( req->query, "name"));
	char* pdata = data;

	str_from_mdb = mdb_find(pdata);
	res_row(a, str_from_mdb);
	//free(str_from_mdb);
}

void mdbinsert(int a, const Request* req)
{
	char ckey[64] = {0};
	char cdata[512] = {0};
	strcpy(ckey, "name");
	strcpy(cdata, hashmap_get( req->query, "name"));
	
	char* key = ckey;
	char* data = cdata;
	mdb_insert(key, data);
	res_row(a, "insert ok!");
}


RouterAdd(mdb){
	ContFun cf[] = {&mdbfind, &mdbinsert, NULL};
	char* keys[] = {"/find", "/insert", NULL};
	router_add_app(cf, keys, __func__);
}

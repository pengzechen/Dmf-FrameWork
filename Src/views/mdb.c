

void mdbfind(int a, const Request* req)
{
	char* str_from_mdb;

	char data[40] = {0};
	char* pdata = data;
	strcpy(data, req->query[0].data);


	str_from_mdb = mdb_find(pdata);
	Res_row(a, str_from_mdb);
	free(str_from_mdb);
}

void mdbinsert(int a, const Request* req)
{
	char* key, *data;
	char ckey[50] = {0};
	char cdata[50] = {0};
	key = ckey;
	data = cdata;
	
	mdb_insert(key, data);
	Res_row(a, "insert ok!");
}


RouterAdd(mdb){
	ContFun cf[] = {&mdbfind, &mdbinsert, NULL};
	char* keys[] = {"/find", "/insert", NULL};
	router_add_app(cf, keys, __func__);
}




void setsession(int a, const Request *req) {
	char res_str[80] = {0};
	strcat(res_str, "SET session ");
	strcat(res_str, req->query[0].key);
	strcat(res_str, " data: ");
	strcat(res_str, req->query[0].data);

	char key[32] = {0}; char data[32] = {0};
	strcat(key, req->query[0].key);
	strcat(data, req->query[0].data);

	char Session_str[11] = {'\0'};
	SessionCreate(Session_str, key, data);

	Response res;
	Res_init(a, &res);
	SetHead(&res, "200");
	SetType(&res, "text/html;utf-8;");
	// SetCookie(&res, "dmfsession", "324fvw3qrc3c23x");
	SetSession(&res, Session_str);
	SetBody(&res, res_str);
	ResParse(&res);
}

void getsession(int a, const Request *req) {
	char key[32] = {0}; char data[32] = {0};
	strcat(key, req->query[0].key);
	strcat(data, req->query[0].data);

	
	char* s = getSessionR(req, data );
	if(s == NULL){
		Res_row(a, "no such key");
	}else{
		char res[256] = {0};
		strcat(res, "session data: ");
		strcat(res, s);
		Res_row(a, res);
	}
}

void sessionadd(int a, const Request *req) {

	char key[32] = {0}; char data[32] = {0};
	strcat(key, req->query[0].key);
	strcat(data, req->query[0].data);

	char res_str[80] = {0};
	strcat(res_str, "SET session ");
	strcat(res_str, key);
	strcat(res_str, " data: ");
	strcat(res_str, data);

	int res = SessionAddR(req, key, data);
	switch(res){
		case -1:
		printf("[view: sessionAdd] req hasn't a session\n");break;
		case -2:
		printf("[view: sessionAdd] session chain hasn't such session\n");break;break;
		default: break;
	}
	
	Res_row(a, res_str);
}

void updatesession(int a, const Request *req) {
	char key[32] = {0}; char data[32] = {0};
	strcat(key, req->query[0].key);
	strcat(data, req->query[0].data);

	int res = UpdateSessionDataR(req, key, data);

	if(res)
		Res_row(a, "Update successfully!");
	else
		Res_row(a, "Update faild");
}

void sessiondebug(int a, const Request* req) {
	SessionAll();
	Res_row(a, "This is a test str");
}

RouterAdd(session){
	ContFun cf[] = {&setsession, &getsession, &sessionadd, &updatesession, &sessiondebug, NULL };
	char* keys[] = {"/setsession", "/getsession", "/sessionadd", "/updatesession", "/sessiondebug", NULL};
	router_add_app(cf, keys, __func__);
}


void setsession(connection_tp conn, const Request *req)
{
	char res_str[80] = {0};

	char * key = hashmap_get( req->query, "key"); 
	char * data = hashmap_get( req->query, "data");
	if ((key == NULL) || (data == NULL)) 
		printf("some error");
		
	strcat(res_str, "SET session ");
	strcat(res_str, key);
	strcat(res_str, " data: ");
	strcat(res_str, data);

	char Session_str[11] = {'\0'};
	SessionCreate(Session_str, key, data);

	response_t res;
	res_init(conn, &res);
	res_set_head(&res, "200");
	res_set_type(&res, "text/html;utf-8;");
	// res_set_cookie(&res, "dmfsession", "324fvw3qrc3c23x");
	res_set_session(&res, Session_str);
	res_set_body(&res, res_str, strlen(res_str));
	res_parse_send(&res);
}

void getsession(connection_tp conn, const Request *req) 
{
	char* data = hashmap_get( req->query, "name");
	
	char* s = getSessionR(req, data);
	if(s == NULL){
		res_row(conn, "no such key");
	}else{
		char res[256] = {0};
		strcat(res, "session data: ");
		strcat(res, s);
		res_row(conn, res);
	}
}

void sessionadd(connection_tp conn, const Request *req) 
{

	char * key = hashmap_get( req->query, "key"); 
	char * data = hashmap_get( req->query, "data");
	if ((key == NULL) || (data == NULL)) 
		printf("some error");

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
		printf("[view: sessionAdd] session chain hasn't such session\n");break;
		default: break;
	}
	
	res_row(conn, res_str);
}

void updatesession(connection_tp conn, const Request *req) 
{
	char * key = hashmap_get( req->query, "key"); 
	char * data = hashmap_get( req->query, "data");
	if ((key == NULL) || (data == NULL)) 
		printf("some error");

	int res = UpdateSessionDataR(req, key, data);

	if(res)
		res_row(conn, "Update successfully!");
	else
		res_row(conn, "Update faild");
}

void sessiondebug(connection_tp conn, const Request* req) 
{
	printf("-----------session debug-------------\n");
	SessionAll();
	printf("-----------session debug-------------\n");
	res_row(conn, "This is a test str");
}

RouterAdd(session)
{
	ContFun cf[] = {&setsession, &getsession, &sessionadd, &updatesession, &sessiondebug, NULL };
	char* keys[] = {"/setsession", "/getsession", "/sessionadd", "/updatesession", "/sessiondebug", NULL};
	router_add_app(cf, keys, __func__);
}
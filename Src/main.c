
#include "server.h"

void setsession(int a, const Request *req) {
	
	Response res;
	Res_init(a, &res);
	SetHead(&res, "200");
	SetType(&res, "text/html;utf-8;");

	// SetCookie(&res, "dmfsession", "324fvw3qrc3c23x");
	SetSession(&res, "login", "true");

	SetBody(&res, "test");
	ResParse(&res);
	// exeSql("select * from test;");
}

void getsession(int a, const Request *req) {
	char *str1;
	for(int i=0; i<=req->p_int; i++) {
		if(strcmp(req->params[i].key, "Cookie")==0) {
			str1=strstr(req->params[i].data, "dmfsession=");
			if(str1 != NULL){
				printf("%s ", str1+11);
				char* session_res = NULL;
				session_res = getSession(str1+11, "login");
				if( session_res == NULL){
					printf("no such key\n");
				}else{
					printf("%s \n", session_res);
				}
			}
		}
	}

	Res_row(a, "This is a test str");
}

void sessiondebug(int a, const Request* req){
	SessionAll();
	Res_row(a, "This is a test str");
}



void mfunction(char *out, char *in) {
	
	strcpy(out, in);
}

void template(int a, const Request *req) {
	struct Kvmap kv[4];
	kv[0].key = "name";
	kv[0].value = "pzcbnvvhjv";
	kv[0].type = 1;
	kv[1].key = "age";
	kv[1].value = "22ghfifuif";
	kv[1].type = 1;
	kv[2].key = "myfunction";
	kv[2].Func = &mfunction;
	kv[2].type = 2;
	kv[3].key = "dec1";
	kv[3].dec[0] = "one";
	kv[3].dec[1] = "two";
	kv[3].dec[2] = "three";
	kv[3].dec[3] = NULL;
	kv[3].type = 3;
	Res_render(a, NULL, kv, 4);
}



void datamodeltest(int a, const Request* req){
	ObjectNode* root = CreateRootNode("root", D_NODE, NULL);
	ObjectNode* mn2 = CreateObjectNode("Child", D_NODE, NULL);
	ObjectNode* mn3 = CreateObjectNode("Bro1", D_INT, (void*)(int*)14);
	ObjectNode* mn4 = CreateObjectNode("Bro2", D_INT, (void*)(int*)146);
	ObjectNode* mn5 = CreateObjectNode("Bro3", D_INT, (void*)(int*)1464);

	AppendChild(root, mn2);
	AppendBro(mn2, mn3);
	AppendBro(mn2, mn4);
	AppendBro(mn2, mn5);


	ShowNodeData(root);
	ShowNodeData(root->mnChild);
	ShowNodeData(root->mnChild->mnBro);
	ShowNodeData(root->mnChild->mnBro->mnBro);
	ShowNodeData(root->mnChild->mnBro->mnBro->mnBro);

	Res_row(a, "This is a test str");
}

void mysqltest(int a, const Request* req) {
	mysql_pool_init();
	mysql_conn* conn1 = get_mysql_connection();
	int aa = mysql_query(&conn1->conn, "select * from test;");
	MYSQL_RES* res_ptr;
	res_ptr = mysql_store_result(&conn1->conn);
	printf("%d %d\n", aa, mysql_num_rows(res_ptr));

	Res_row(a, "ok");
	release_mysql_connection(conn1);
}




int main() {
	ContFun cf[] = {&getsession, &template, &setsession, &sessiondebug, &mysqltest};
	char* keys[] = {"/getsession", "/template", "/setsession", "/sessiondebug", "/mysqltest"};
	// SimpleServerMake(cf, keys);
	// SSLservermake(cf, keys);
	
	ContFunMap cmp;
	cmp.cf[0] = &getsession;
	cmp.cf[1] = &template;
	cmp.cf[2] = &setsession;
	cmp.cf[3] = &sessiondebug;
	cmp.cf[4] = &mysqltest;
	cmp.cf[5] = &datamodeltest;
	cmp.cf[6] = NULL;
	cmp.keys[0] = "/getsession";
	cmp.keys[1] = "/template";
	cmp.keys[2] = "/setsession";
	cmp.keys[3] = "/sessiondebug";
	cmp.keys[4] = "/mysqltest";
	cmp.keys[5] = "/datamodeltest";
	cmp.keys[6] = NULL;

	iocpServerMake(cmp);
	
	return 0;
}
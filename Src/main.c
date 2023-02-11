
#include "server.h"

void func1(int a, const Request *req) {
	for(int i=0; i<=req->p_int; i++){
		if(strcmp(req->params[i].key, "Cookie")==0){
			printf("%s\n", req->params[i].data);
		}
	}

	// char session_res[100] = {'\0'}; 
	// strcpy(session_res, getSession(key, "name"));
	// printf("%s", session_res);
	Res_row(a, "This is a test str");
}

void mfunction(char *out, char *in) {
	
	strcpy(out, in);
}

void func2(int a, const Request *req) {
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

void func3(int a, const Request *req) {
	
	Response res;
	Res_init(a, &res);
	SetHead(&res, "200");
	SetType(&res, "text/html;utf-8;");

	// SetCookie(&res, "dmfsession", "324fvw3qrc3c23x");
	SetSession(&res, "name", "pengzechen");

	SetBody(&res, "test");
	ResParse(&res);
	
	// exeSql("select * from test;");
}

void sessiontest(int a, const Request* req){

	SessionAll();
	Res_row(a, "This is a test str");
}


int main() {
	ContFun cf[] = {&func1, &func2, &func3, &sessiontest};
	char* keys[] = {"/func1", "/func2", "/func3", "/sessiontest"};
	// SimpleServerMake(cf, keys);

	
	
	ContFunMap cmp;
	cmp.cf[0] = &func1;
	cmp.cf[1] = &func2;
	cmp.cf[2] = &func3;
	cmp.cf[3] = &sessiontest;
	cmp.cf[4] = NULL;
	
	cmp.keys[0] = "/func1";
	cmp.keys[1] = "/func2";
	cmp.keys[2] = "/func3";
	cmp.keys[3] = "/sessiontest";
	cmp.keys[4] = NULL;
	
	
	iocpServerMake(cmp);
	
	return 0;
}
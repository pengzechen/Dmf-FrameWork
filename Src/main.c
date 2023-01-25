

#include "server.h"


void func1(int a, const Request *req) {
	
	Res_row(a, "This is a test str");
}


void mfunction(char *out, char *in) {
	// 
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
	SetCookie(&res, "dmf", "cookie data");
	SetBody(&res, "test");
	ResParse(&res);
	
	exeSql("select * from test;");
}


ContFun cf[] = {&func1, &func2, &func3};


char* keys[] = {"/func1", "/func2", "/func3"};


int main() {
	SimpleServerMake(cf, keys);
	return 0;
}
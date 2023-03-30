#include <model/data.h>
#include <request.h>

void datamodeltest(int a, const Request* req) 
{
	int i = 0;
	objPtr root = CreateObjectNode("root", "hhh");
	objPtr mn1 = CreateObjectNode("Bro0", "ppp");
	objPtr mn2 = CreateObjectNode("Bro1","sss");
	objPtr mn3 = CreateObjectNode("Bro2","sss1");
	objPtr mn4 = CreateObjectNode("Bro3","sss2");
	
	
	AppendChild(root, mn1);
	AppendBro(mn1, mn2);
	AppendBro(mn1, mn3);
	AppendBro(mn1, mn4);


	ShowNodeData(root);
	printf("\n");
	ShowNodeData(mn1);
	printf("\n");

	Res_row(a, "This is a test str");
}

void elrtest(int a, const Request* req)
{
	//char* str = (char*) malloc (sizeof(char)* 50);

	//free(str);
	//printf("g_server_conf_all: database %s\n", g_server_conf_all._conf_model.database);

	printf("%s\n", req->query[0].key);

	Res_row(a, "test ok");
}

RouterAdd(other){
	ContFun cf[] = {&datamodeltest, &elrtest, NULL};
	char* keys[] = {"/datamodeltest", "/elrtest", NULL};
	router_add_app(cf, keys, __func__);
}
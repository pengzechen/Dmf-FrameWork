#include <request.h>
#include <response.h>
#include <model/data.h>


void datamodeltest(int a, const Request* req) 
{
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

void elrtest(int a, const Request* req)
{
	//char* str = (char*) malloc (sizeof(char)* 50);

	//free(str);
	printf("g_server_conf_all: database %s\n", g_server_conf_all._conf_model.database);

	Res_row(a, "test ok");
}
/* 
Copyright 2023 Ajax

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
*/
#include "server.h"



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

void mysqltest(int a, const Request* req) 
{
	
	mysql_conn* conn1 = get_mysql_connection_block();
	mysql_query(&conn1->conn, "select * from test;");
	MYSQL_RES* res_ptr = mysql_store_result(&conn1->conn);
	//printf("   query result %d\n", mysql_num_rows(res_ptr));
	mysql_free_result(res_ptr);
	free(res_ptr);

	Res_row(a, "ok");
	release_mysql_connection(conn1);
}

void mysqltest1(int a, const Request* req) 
{
	
	exeSql("select * from test;");

	Res_row(a, "ok");
}


void elrtest(int a, const Request* req)
{
	//char* str = (char*) malloc (sizeof(char)* 50);

	//free(str);
	printf("g_server_conf_all: database %s\n", g_server_conf_all._conf_model.database);

	Res_row(a, "test ok");
}



#include "./views/session.c"
#include "./views/template.c"

#include <link.h>

int main(int arg, char* args[]) {
	ConfInit();
	SessionInit();
	mysql_pool_init();
	elr_mpl_init();

	HMODULE handle = LoadLibrary("./link/liblink.dll");
	Get link_get = (Get)GetProcAddress(handle, "get");
	Set link_set = (Set)GetProcAddress(handle, "set");
	Dll_read_shm dll_read_shm = (Dll_read_shm)GetProcAddress(handle, "dll_read_shm");
	dll_read_shm();


	ContFun cf[] = {&getsession, &template, &setsession, &sessiondebug, 
					&mysqltest, &datamodeltest, &elrtest, &sessionadd, &updatesession, NULL};
	char* keys[] = {"/getsession", "/template", "/setsession", "/sessiondebug", 
					"/mysqltest", "/datamodeltest", "/elrtest", "/sessionadd", "/updatesession", NULL};
	
	// SimpleServerMake(cf, keys);
	// SSLservermake(cf, keys);
	
	ContFunMap cmp;
	cmp.cf[0] = &getsession;
	cmp.cf[1] = &template;
	cmp.cf[2] = &setsession;
	cmp.cf[3] = &sessiondebug;
	cmp.cf[4] = &mysqltest;
	cmp.cf[5] = &datamodeltest;
	cmp.cf[6] = &elrtest;
	cmp.cf[7] = &sessionadd;
	cmp.cf[8] = &updatesession;
	cmp.cf[9] = NULL;
	cmp.keys[0] = "/getsession";
	cmp.keys[1] = "/template";
	cmp.keys[2] = "/setsession";
	cmp.keys[3] = "/sessiondebug";
	cmp.keys[4] = "/mysqltest";
	cmp.keys[5] = "/datamodeltest";
	cmp.keys[6] = "/elrtest";
	cmp.keys[7] = "/sessionadd";
	cmp.keys[8] = "/updatesession";
	cmp.keys[9] = NULL;
	iocpServerMake(cmp);

	
	return 0;
}
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
#include <link.h>

#include "./views/session.c"
#include "./views/template.c"
#include "./views/mysql.c"


int main(int arg, char* args[]) {
	ConfInit();
	SessionInit();
	mysql_pool_init();
	elr_mpl_init();
	
	ShellExecute(NULL, "open", "linktest.exe", NULL, NULL, SW_HIDE);
	Sleep(500);
	HMODULE handle = LoadLibrary("./link/liblink.dll");
	Get link_get = (Get)GetProcAddress(handle, "get");
	Set link_set = (Set)GetProcAddress(handle, "set");
	Dll_read_shm dll_read_shm = (Dll_read_shm)GetProcAddress(handle, "dll_read_shm");
	dll_read_shm();
	Dll_write_shm dll_write_shm = (Dll_write_shm)GetProcAddress(handle, "dll_write_shm");
	dll_write_shm("rewrite shm data");

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
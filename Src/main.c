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


#include <dmfserver/server.h>

#include <dmfserver/session.h>			// 初始化 session
#include <dmfserver/cpool.h>				// 初始化 mysqlpool
#include <dmfserver/conf/conf.h>			// 初始化 全局配置
#include <elr_mpl/elr_mpl.h>	// 初始化 内存池

#include "./views/session.c"
#include "./views/template.c"
#include "./views/mysql.c"
#include "./views/other.c"

#include <dmfserver/link.h>

#ifdef __WIN32__
void linkload(){
	ShellExecute(NULL, "open", "linktest.exe", NULL, NULL, SW_SHOW);
	Sleep(500);
	HMODULE handle = LoadLibrary("./link/liblink.dll");
	Get link_get = (Get)GetProcAddress(handle, "get");
	Set link_set = (Set)GetProcAddress(handle, "set");
	Dll_read_shm dll_read_shm = (Dll_read_shm)GetProcAddress(handle, "dll_read_shm");
	dll_read_shm();
	Dll_write_shm dll_write_shm = (Dll_write_shm)GetProcAddress(handle, "dll_write_shm");
	dll_write_shm("rewrite shm data");
}
#endif //WIN32


int main(int argc, char* argv[]) {
	#ifdef __WIN32__
	system("cls");
	system("tasklist /nh | find /i \"mysqld.exe\"");
	#endif // WIN32

	ConfInit();
	SessionInit();
	Router_init();
	mysql_pool_init();
	elr_mpl_init();

	model();
	other();
	session();
	apptemp();

	ContFun cf[] = {&getsession, &template, &setsession, &sessiondebug, 
					&mysqltest, &datamodeltest, &elrtest, &sessionadd, 
					&updatesession, NULL};
	char* keys[] = {"/getsession", "/template", "/setsession", "/sessiondebug", 
					"/mysqltest", "/datamodeltest", "/elrtest", "/sessionadd",
					"/updatesession", NULL};
	

#ifdef __WIN32__
	iocpServerMake(g_cmp);
	// SimpleServerMake(cf, keys);
	// SSLservermake(cf, keys);
#elif __linux__ // linux
	SimpleServerMake(cf, keys);
#endif // linux
	return 0;
}
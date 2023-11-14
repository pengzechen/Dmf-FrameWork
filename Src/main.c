/* 
    *  Copyright 2023 Ajax
    *
    *  Licensed under the Apache License, Version 2.0 (the "License");
    *  you may not use this file except in compliance with the License.
    *
    *  You may obtain a copy of the License at
    *
    *    http://www.apache.org/licenses/LICENSE-2.0
    *    
    *  Unless required by applicable law or agreed to in writing, software
    *  distributed under the License is distributed on an "AS IS" BASIS,
    *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    *  See the License for the specific language governing permissions and
    *  limitations under the License. 
    *
    */

/*  
    *                       MAIN
    *
    *   It's show the simple usage of this server:    
    *       (1) init server
    *       (2) load your own view
    *       (3) start the server
    *       (4) clean up resource
    */

#include <dmfserver/server.h>

#include <dmfserver/session.h>			// 初始化 session
#include <dmfserver/cpool.h>				// 初始化 mysqlpool
#include <dmfserver/conf/conf.h>			// 初始化 全局配置

#include <dmfserver/elr_mpl/elr_mpl.h>	// 初始化 内存池
#include <dmfserver/mpool.h>
#include <dmfserver/mdb/mdb_operate.h>
#include <dmfserver/utility/dm_map.h>

#include <stdlib.h>


#include "./testviews/session.c"
#include "./testviews/template.c"
#include "./testviews/mysql.c"
#include "./testviews/other.c"
#include "./testviews/mdb.c"
#include "./testviews/ws.c"


#define OVECCOUNT 30 /* should be a multiple of 3 */
#define EBUFLEN 128
#define BUFLEN 1024

typedef void (*worker_function)();
void multi_process_init(worker_function _wf);


int main(int argc, char* argv[]) 
{
    test_map();
#if 1
	#ifdef __WIN32__
		// system("cls");
		// system("tasklist /nh | find /i \"mysqld.exe\"");
		// ShowWindow(GetConsoleWindow(), SW_HIDE);
		// FreeConsole();
	#endif // WIN32


    #ifdef __SERVER_MPOOL__
        printf("server mpool start !\n");
    #endif // __SERVER_MPOOL__

    // 安装信号
    // signal(SIGINT, handle_signal);
    // signal(SIGTERM, handle_signal);

    conf_init();        // 框架参数初始化
    log_init();         // 日志记录模块初始化
    middleware_init();  // 中间件初始化
    session_init();     // session 模块初始化
    template_init();    // 模板模块初始化
    router_init();      // 路由模块初始化

    mysql_pool_init();  // mysql 连接池初始化
    elr_mpl_init();     // 内存池初始化

    pool_init(2076, 2076*8192);  // server 模块内存池初始化
    pool_init2(8, 8*8192);       // server 模块内存池初始化
    
    mdb_operate_init();          // mdb 模块初始化


    // *以下载入 views 的函数，
    // *载入以后，router 将根据载入的函数调用相对应的 view
    model();
    other();
    session();
    apptemp();
    mdb();
    ws();

    // 根据使用的平台启动服务器
#ifdef __WIN32__	// Win32
	iocp_server_make();
	// simple_server_make();
	// simple_ssl_server_make();
#elif __linux__ 	// linux
	// simple_server_make();
	// simple_ssl_server_make();
	epoll_server_make();
    // multi_process_init(&epoll_server_make);
	// epoll_ssl_server();
#endif 				// linux

    // 平滑退出时做相应的清理工作
	pool_destroy();
    pool_destroy2();
    template_free();

#endif

	return 0;
}
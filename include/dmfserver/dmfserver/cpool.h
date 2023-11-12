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

#ifndef __MYSQL_POOL_INCLUDE__
#define __MYSQL_POOL_INCLUDE__


#include <mysql/mysql.h>
#include <pthread.h>
#include <dmfserver/conf/conf.h>
#define MAX_KEEP_CONNECTIONS 4

//定义mysql连接链表结构
typedef struct mysql_conn     
{
	struct mysql_conn * next;   //下一个连接的指针
	struct mysql_conn * prev;   //上一个的指针
	MYSQL conn;

} mysql_conn;    //相当于typedef struct mysql_conn mysql_conn;  第一个mysql_conn为类型，第二个为变量名



//定义mysql连接池结构
typedef struct mysql_pool    
{
	char host[64];			//主机名
	char username[32];		//用户名
	char password[32];		//密码
	char database[32];		//默认数据库
	int port;				//端口号 默认3306
	int min_connections;	//最小连接数
	int max_connections;	//最大连接数
	int free_connections;	//保持空闲的最大连接数

	
	pthread_mutex_t lock;	//mysql链表锁

	int is_idle_block;			//是否开启无可用 连接阻塞
	pthread_cond_t idle_signal;	//等待可用连接的条件变量

	mysql_conn * mysql_list; 	//mysql连接池链表

} mysql_pool;

#ifdef __cplusplus
extern "C" {
#endif

void mysql_pool_init();   				//执行初始化方法
void destroy_mysql_connection(mysql_conn *conn);
void destroy_mysql_pool();
void release_mysql_connection(mysql_conn *conn);
mysql_conn * get_mysql_connection();
mysql_conn * get_mysql_connection_block();
MYSQL_RES * mysql_execute_query(const char *sql,unsigned long length,int * flag);


#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif  //__MYSQL_POOL_INCLUDE__


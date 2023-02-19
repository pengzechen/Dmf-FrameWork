#ifndef MYSQL_POOL_H
#define MYSQL_POOL_H
#include <mysql/mysql.h>
#include <pthread.h>
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
	int is_idle_block;		//是否开启无可用连接阻塞
	pthread_mutex_t lock;	//mysql链表锁
	pthread_cond_t idle_signal;	//等待可用连接的条件变量
	mysql_conn * mysql_list; //mysql连接池链表
} mysql_pool;

void mysql_pool_init();   //执行初始化方法
void destroy_mysql_connection(mysql_conn *conn);
void destroy_mysql_pool();
void release_mysql_connection(mysql_conn *conn);
mysql_conn * get_mysql_connection();

MYSQL_RES * mysql_execute_query(const char *sql,unsigned long length,int * flag);

#endif  //MYSQL_POOL_H


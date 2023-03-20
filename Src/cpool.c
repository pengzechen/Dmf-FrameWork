
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "cpool.h"

static mysql_pool pool_mysql; //连接池定义
unsigned int query_times = 0; //mysql所有的查询次数，用于测试


//创建一个新的mysql连接
mysql_conn * mysql_new_connection()
{
	mysql_conn * conn=malloc(sizeof(mysql_conn));    //定义一个类型为mysql_conn的名为conn的指针变量,分配内存
	if (mysql_init(&conn->conn) == NULL)
	{
		printf("can not init mysql: [%s]\n",strerror(errno));
		free(conn);
		return NULL;
	}

	if(mysql_options(&conn->conn,MYSQL_SET_CHARSET_NAME,"utf8") != 0) //设置字符编码
	{
		printf("can not set mysql options[errno = %d]: [%s]\n",mysql_errno(&conn->conn),mysql_error(&conn->conn));
		free(conn);
		return NULL;
	}

	//连接到mysql服务端
	if(mysql_real_connect(&conn->conn,pool_mysql.host,pool_mysql.username,
		pool_mysql.password,pool_mysql.database,pool_mysql.port,NULL,
		CLIENT_MULTI_STATEMENTS) == NULL)
	{
		printf("can not connect mysql server[errno = %d]:[%s]\n",mysql_errno(&conn->conn),mysql_error(&conn->conn));
		free(conn);
		return NULL;
	}

	conn->next = NULL;
	conn->prev = NULL;
	return conn;
}


//向连接池中加入一个mysql连接conn
void conn_push(mysql_conn * conn)
{
	mysql_conn *lc = pool_mysql.mysql_list;   //*lc指针变量
	if (lc == NULL)
	{
		pool_mysql.mysql_list = conn;     //如果连接池为空，直接把第一个放进去
	}else
	{
		while(lc->next)    //循环到末尾，直到next属性为null
		{
			lc=lc->next;
		}

		lc->next = conn;   //连接池的下一个是当前连接conn
		conn->prev = lc;   //当前连接的上一个是过去的lc
	}
	pool_mysql.free_connections++;
}


//从连接池中取出一个mysql连接,返回类型为mysql_conn,如果没有可用的连接，返回null。
mysql_conn * conn_pop()
{
	mysql_conn * conn = pool_mysql.mysql_list;
	if (conn != NULL)
	{
		pool_mysql.mysql_list = conn->next;    //如果只有一个，next是null,这样会有问题吧？
		if (pool_mysql.mysql_list)
		{
			pool_mysql.mysql_list->prev = NULL;
		}
		pool_mysql.free_connections--;
	}
	return conn;
}

//初始化mysql连接池
void mysql_pool_init()
{
	mysql_conn * conn;    //定义一个conn变量，他是指向mysql_conn变量的指针
	strncpy(pool_mysql.host, server_conf_all._conf_model.host, sizeof(pool_mysql.host));
	strncpy(pool_mysql.username, server_conf_all._conf_model.username, sizeof(pool_mysql.username));
	strncpy(pool_mysql.password, server_conf_all._conf_model.password, sizeof(pool_mysql.password));
	strncpy(pool_mysql.database, server_conf_all._conf_model.database, sizeof(pool_mysql.database));

	pool_mysql.port = server_conf_all._conf_model.port;
	pool_mysql.max_connections = MAX_KEEP_CONNECTIONS;
	pool_mysql.free_connections = 0;
	pool_mysql.mysql_list = NULL;
	pool_mysql.is_idle_block = 0;

	pthread_mutex_init(&pool_mysql.lock,NULL);
	pthread_cond_init(&pool_mysql.idle_signal,NULL);
	pthread_mutex_lock(&pool_mysql.lock);
	pool_mysql.min_connections = 20;

	for (int i = 0; i < pool_mysql.min_connections; ++i)    //初始化时创建几个连接并加入池中
	{
		conn = mysql_new_connection();
		if (conn)
		{
			conn_push(conn);
		}
	}

	pthread_mutex_unlock(&pool_mysql.lock);

	printf("[Server: Info] mysqlpool init successfully... %d connections is ok! \n", pool_mysql.min_connections);
}


//从连接池中取出一个mysql连接
mysql_conn * get_mysql_connection()
{
	pthread_mutex_lock(&pool_mysql.lock);
	mysql_conn *conn = conn_pop();
	pthread_mutex_unlock(&pool_mysql.lock);
	return conn;
}


//从连接池中取出一个mysql连接，如果为空，则阻塞。
mysql_conn * get_mysql_connection_block()
{
	pthread_mutex_lock(&pool_mysql.lock);
	mysql_conn *conn = conn_pop();
	// printf("current free connections %d\n ", pool_mysql.free_connections);
	if (conn == NULL)
	{
		pool_mysql.is_idle_block++;
		pthread_cond_wait(&pool_mysql.idle_signal,&pool_mysql.lock);
		conn = conn_pop();
		pool_mysql.is_idle_block--;
	}
	query_times++;
	pthread_mutex_unlock(&pool_mysql.lock);
	return conn;
}


//回收一个mysql连接到连接池
void release_mysql_connection(mysql_conn *conn)
{
	pthread_mutex_lock(&pool_mysql.lock);
	conn->next = NULL;
	conn->prev = NULL;
	conn_push(conn);
	if(pool_mysql.is_idle_block)
	{
		pthread_cond_signal(&pool_mysql.idle_signal);
	}
	pthread_mutex_unlock(&pool_mysql.lock);
}


//关闭一个mysql连接并销毁内存占用
void destroy_mysql_connection(mysql_conn *conn)
{
	mysql_close(&conn->conn);
	free(conn);
}


//销毁连接池中所有连接
void destroy_mysql_pool()
{
	mysql_conn *conn;
	pthread_mutex_lock(&pool_mysql.lock);
	conn = conn_pop();
	for (;conn;conn=conn_pop())
	{
		destroy_mysql_connection(conn);
	}

	pthread_mutex_unlock(&pool_mysql.lock);
}


//执行mysql语句
MYSQL_RES * mysql_execute_query(const char *sql,unsigned long length,int *flag)
{
	int res;
	MYSQL_RES *res_ptr;
	mysql_conn *con = get_mysql_connection_block();
	if (con == NULL)
	{
		printf("can not get mysql connections from the pool\n");
		*flag = -2;
		return NULL;
	}

	*flag = 0;
	res = mysql_real_query(&con->conn,sql,length);   //执行sql语句，成功返回0，否则返回非0的整数
	if (res != 0)
	{
		printf("mysql_real_query error [errno=%d]:[%s]\n",mysql_errno(&con->conn),mysql_error(&con->conn));
		release_mysql_connection(con);
		*flag=res;
		return NULL;
	}

	res_ptr = mysql_store_result(&con->conn);
	if (res_ptr == NULL)
	{
		printf("mysql_store_result error [errno = %d]:[%s]\n",mysql_errno(&con->conn),mysql_error(&con->conn));
	}

	release_mysql_connection(con);
	return res_ptr;
}
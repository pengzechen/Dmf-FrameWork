
#ifndef MODEL
#define MODEL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*引入连接Mysql的头文件和lib包*/
#include "mysql/mysql.h"
#pragma comment(lib,"libmysql.lib")

/*定义一些数据库连接需要的宏*/
#define HOST "localhost" 			/*MySql服务器地址*/
#define USERNAME "root" 			/*用户名*/
#define PASSWORD "dmfserver2023" 	/*数据库连接密码*/
#define DATABASE "test" 			/*需要连接的数据库*/

typedef struct _Model {
	MYSQL* my_connection;
	MYSQL_RES* res_ptr;
	int row;
	int column;
	
} Model;

typedef Model* ModelPtr;


typedef struct _ModelData {
	int column;
	int type;

	
	char* Cdata;			// type1
	int Idate;
	long int  LIdata;
	double Ddata;
	long double LDdata;		// type5
} ModelData;

typedef struct _testModel {
	ModelData *md;
	
	int id;
	
} testModel;


//  mysql_affected_rows(&my_connection)  返回int

extern void exeSql(char* sql);



#endif
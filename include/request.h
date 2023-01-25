
#ifndef REQUEST
#define REQUEST

#define MULTI_DEBUG 0
#define REQUEST_DEBUG 0

#define BODY_MAX 1024*1024				// body 数据大小
#define MULTI_PART_MAX 1024*1024		// multipart 数据大小
#define MULTI_PART_MAX_NUM 20			// 最大multipart 数量

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


struct Multi_kv {
	char key[32];
	char data[64];
};

struct Multipart {
	struct Multi_kv name;
	struct Multi_kv dis;
	struct Multi_kv filename;
	char *data;
	int length;
};

struct Query {
	char key[64];
	char data[512];
};

struct Params {
	char key[64];
	char data[256];
};

struct Body {
	char *body;
	int length;
};

struct req {
	char method[5];
	char path[512];
	char protocol[5];
	char version[4];
	
	int q_int;
	int p_int;
	int multi_part_num;
	
	struct Query query[10];
	struct Params params[20];
	struct Body body;
	
	struct Multipart *multi[MULTI_PART_MAX_NUM];
};

typedef struct req Request;

static void MultiParse (Request *request, char *boundary );

void ParseHttp(Request *request, char *data);

void freeReq(Request *req);

#endif
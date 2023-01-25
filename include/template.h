
#ifndef TEMPLATE
#define TEMPLATE


#define TEMPLATE_RESULT_SIZE 4096 
#define DEC_NUM 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Kvmap {
	int type;
	char* key;
	char* value;					// #	1
	
	char* dec[ DEC_NUM ];
	void (*Func)(char*, char *);			// 
};


char * loadTemplate(char * template_path);

static void ParseDec(char* tt, char* dec[], char* inner);

char * parseContext(char *context, struct Kvmap *kv, int kv_num);


#endif
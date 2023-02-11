#ifndef SESSION
#define SESSION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


#define HASH_DEC_LEN 19


// Hash Map defination 
typedef struct HashNode
{
    char* key;
    char* value;
    struct HashNode* next; // 当key相同时，指向集合中的下一个节点

} HashNode ;

// 全局变量
HashNode session_all_dec[ HASH_DEC_LEN ];

extern void SessionInit(HashNode* session_all_dec);

void SessionCreate(char* ,char*, char*);

extern void SessionAll();

char* getSession(char*, char*);



#endif /* SESSION */
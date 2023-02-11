#ifndef SESSION
#define SESSION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


#define HASH_MXA_LEN 19



// Hash Map defination 
typedef struct HashNode
{
    char* key;
    char* value;
    struct HashNode* next; // 当key相同时，指向集合中的下一个节点
}HashNode;





void sessionInit(char* );

char* getSession(char*, char*);

#endif /* SESSION */
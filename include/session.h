#ifndef SESSION
#define SESSION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


#define HASH_DEC_LEN 19

typedef struct _SessionData {
    char* key;
    char* data;
    struct _SessionData* next;
} SessionData;

// Hash Map defination 
typedef struct _HashNode
{
    char* key;
    SessionData* value;
    struct _HashNode* next; // 当下标相同时，指向集合中的下一个节点

} HashNode ;

// 全局变量
HashNode session_all_dec[ HASH_DEC_LEN ];

extern void SessionInit(HashNode* session_all_dec);

void SessionCreate(char* ,char*, char*);

extern void SessionAll();

char* getSession(char*, char*);



#endif /* SESSION */
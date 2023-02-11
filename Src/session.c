
#include "session.h"

/*

// the max number of key_values in each session file
#define SIGNAL_SESSION_MAX_NUM 32
#define SESSION_DIR "./sessions/"

#define SESSION_NAME_LEN 32

typedef enum _SessionType {
    Sint = 1, Sstring, Sfloat
} SessionType;

typedef struct _SessionPart {
    char name[ SESSION_NAME_LEN ];
    SessionType session_type;
    unsigned int start;
    unsigned int len;
} SessionPart ;

*/

/*


// create a empty session file
int createSession(char * session_name) {
	
    // splicing the session path and session_name(filename)
    char path_name[64];
    memset(path_name, 0, 64);
    strcat(path_name, SESSION_DIR);
    strcat(path_name, session_name);

    FILE *fp;
	fp = fopen( path_name, "w" );
	if(fp == NULL){ printf(" open Failed");  }

    int num = -1;
    fwrite(&num, sizeof(int), 1, fp);

	for(int i=0; i < SIGNAL_SESSION_MAX_NUM; i++){
        SessionPart sp;
        memset(sp.name, 0, SESSION_NAME_LEN);
        sp.start = 0;
        sp.len = 0;
        sp.session_type = Sstring; // default c_string
        fwrite(&sp, sizeof(SessionPart), 1, fp);
    }
	fclose(fp);

    return 0;
}

int sessionAdd(char session_name, char key, char* value, SessionType type, int len)
{
    // splicing the session path and session_name(filename)
    char path_name[64];
    memset(path_name, 0, 64);
    strcat(path_name, SESSION_DIR);
    strcat(path_name, session_name);

    FILE *fp;
	fp = fopen( path_name, "w+" );
	if(fp == NULL){
        printf("open Failed \n");  
        return 1;
    }

    int current_num;
    fread(current_num, sizeof(int), 1, fp);
    current_num ++;

    SessionType temp;
    
    for(int i=0; i < current_num; i++){
        
    }

}

*/


static int get_random_str(char* random_str, const int random_len)
{
    int i, random_num, seed_str_len;
    struct timeval tv;
    unsigned int seed_num;
    char seed_str[] = "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    seed_str_len = strlen(seed_str);

    gettimeofday(&tv, NULL);
    seed_num = (unsigned int)(tv.tv_sec + tv.tv_usec);
    srand(seed_num);

    for(i = 0; i < random_len; i++)
    {
        random_num = rand()%seed_str_len;
        random_str[i] = seed_str[random_num];
    }

    return 0;
}

// BKDR Hash Function
static unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131;   // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

extern void SessionInit(HashNode* session_all_dec)
{
    for(int i=0; i<HASH_DEC_LEN; i++) {
        session_all_dec[i].key = NULL;
        session_all_dec[i].value = NULL;
        session_all_dec[i].next = NULL;
    }
    printf("[Server: Info] session init successfully...\n");
}

static HashNode* CreateNewHashNode()
{
    HashNode *p = malloc(sizeof(HashNode)*1);
    if(p == NULL){
        return NULL;
    }else{
        p->key = NULL;
        p->next = NULL;
        p->value = NULL;
    }
    return p;
}

extern void SessionCreate(char* random_str,char* key, char* value) {
    
    get_random_str(random_str, 10);

    unsigned int a = BKDRHash(random_str);

    printf("%s, Hash int %d\n", random_str, a % HASH_DEC_LEN);

    time_t session_create_time;
	session_create_time = time(NULL);
	int ii = time(&session_create_time);
    


    HashNode* new_node = CreateNewHashNode();
    new_node->key = malloc(sizeof(char)*strlen(key));
    new_node->value = malloc(sizeof(char)*strlen(value));
    strcpy(new_node->key, key);
    strcpy(new_node->value, value);

    if(session_all_dec[a % HASH_DEC_LEN].next == NULL){
        // 某个节点第一次添加数据
        session_all_dec[a % HASH_DEC_LEN].next = new_node;
    }else{
        // 从头部插入新节点
        new_node->next = session_all_dec[a % HASH_DEC_LEN].next;
        session_all_dec[a % HASH_DEC_LEN].next = new_node;
    }
        
}

// debug 使用
extern void SessionAll(){
    HashNode* temp;

    for(int i=0; i< HASH_DEC_LEN; i++){
        temp = &session_all_dec[i];
        printf("%d: ", i);
        while(temp->next != NULL){
            temp = temp->next; 
            printf(" [%s: %s]->", temp->key, temp->value);
        }
        printf("\n");
    }
}


char* getSession(char* session_str, char* key) {
    // 计算当前 session 序号是多少
    unsigned int index = BKDRHash(session_str) % HASH_DEC_LEN;
    HashNode* temp;
    temp = &session_all_dec[index];
    while(temp->next != NULL){
        temp = temp->next; 
        if(strcmp(temp->key, key)==0){
            return temp->value;
        }
    }
    return NULL;
}

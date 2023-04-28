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


#include <dmfserver/session.h>

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

// 在session模块中使用的全局变量
static HashNode g_session_all_dec[ HASH_DEC_LEN ];

// 产生随机的字符串
static int get_random_str(char* random_str, const int random_len)
{
    int i, random_num, seed_str_len;

    unsigned int seed_num;
    char seed_str[] = "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    seed_str_len = strlen(seed_str);

    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep); //取得当地具体时间

    seed_num = (unsigned int)(p->tm_min + p->tm_sec);
    srand(seed_num);

    for(i = 0; i < random_len; i++){

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
    while (*str){
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}


void* session_write_file() {
    // while(1){
    //     SessionAll();
    // }
}


extern void session_init()
{
    for(int i=0; i<HASH_DEC_LEN; i++) {
        g_session_all_dec[i].key = NULL;
        g_session_all_dec[i].value = NULL;
        g_session_all_dec[i].next = NULL;
    }
    printf("[Session: Info] session init successfully...\n");
    printf("\n");

    pthread_t roundCheck;
	pthread_create(&roundCheck, NULL, session_write_file, NULL);

    // pthread_join(roundCheck, NULL);
}


static HashNode* CreateNewHashNode()
{
    HashNode *p = (HashNode*)malloc(sizeof(HashNode)*1);
    if(p == NULL){
        return NULL;
    }else{
        p->key = NULL;
        p->next = NULL;
        p->value = NULL;
    }
    return p;
}


// debug 使用
extern void SessionAll()
{
    HashNode* temp;
    SessionData* dtemp;
    char buff[10] = {0};
    time_t t = time(NULL);
    char str_res[1024] = {0};
    

    for(int i=0; i< HASH_DEC_LEN; i++){
        temp = &g_session_all_dec[i];

        itoa(i,buff,10);
        strcat(str_res, buff);
        memset(buff, 0, 10);
        strcat(str_res, "------------\n");

        while(temp->next != NULL) {
            temp = temp->next; 
            strcat(str_res, temp->key);
            strcat(str_res, "\n");

            dtemp = temp->value;
            while((dtemp) != NULL){
                
                strcat(str_res, "           Key: ");
                strcat(str_res, dtemp->key);
                strcat(str_res, " Data: ");
                strcat(str_res, dtemp->data);

                if(dtemp->time_store + dtemp->expire < time(&t))
                strcat(str_res, "(expired)");

                strcat(str_res, "\n");

                dtemp = dtemp->next;
            }

            strcat(str_res, "\n");
        }
    }

    printf("%s\n", str_res);
}


// 创建一个新的session并往session里面加一个sessiondata 
extern void SessionCreate(char* random_str, char* key, char* value) 
{
    
    get_random_str(random_str, 10);
    unsigned int a = BKDRHash(random_str);
    printf("%s, Hash int %d\n", random_str, a % HASH_DEC_LEN);

    //time_t session_create_time;
	//session_create_time = time(NULL);
	//int ii = time(&session_create_time);

    // 创建一个 HashNode 并存放一个 SessionData 
    // HashNode->next = NULL   SessionData->next = NULL
    HashNode* new_node = CreateNewHashNode();  // HashNode->next = NULL
    new_node->key = malloc(sizeof(char)*strlen(random_str));      // session code
    strcpy(new_node->key, random_str);
    new_node->value = (SessionData*)malloc(sizeof(SessionData));    // session data
    new_node->value->key = (char*)malloc(sizeof(char)*strlen(key));       // malloc data name
    new_node->value->data = (char*)malloc(sizeof(char)*strlen(value));    // malloc data 
    strcpy(new_node->value->key, key);      // set data name
    strcpy(new_node->value->data, value);   // set data
    time_t t = time(NULL);
    new_node->value->time_store = time(&t);
    new_node->value->expire = SESSION_EXPIRE_DEFAULT;
    new_node->value->next = NULL;


    if(g_session_all_dec[a % HASH_DEC_LEN].next == NULL){
        // 某个节点第一次添加数据
        g_session_all_dec[a % HASH_DEC_LEN].next = new_node;        // g_session_all_dec[通过 session code 得出的下标]
    }else{
        // 从头部插入新节点
        new_node->next = g_session_all_dec[a % HASH_DEC_LEN].next;
        g_session_all_dec[a % HASH_DEC_LEN].next = new_node;
    }
        
}


/*  \brief 通过 session_str 和数据名得到对应数据 
    \param 传入 req   */
extern char* getSessionR(const Request* req, char* key) 
{
    char *str1;
    char* session_res = NULL;

	for(int i=0; i<=req->p_int; i++) {
		if(strcmp(req->params[i].key, "Cookie")==0) {
			str1 = strstr(req->params[i].data, "dmfsession=");
            *(str1 + 21) = '\0';
			if(str1 != NULL){
				session_res = getSession(str1+11, key);
                return session_res == NULL ? NULL : session_res;
			}
		}
	}

    return NULL;
}


/* \brief 通过 session_str 和数据名得到对应数据 */
extern char* getSession(char* session_str, char* key) 
{
    // 计算当前 session 序号是多少
    unsigned int index = BKDRHash(session_str) % HASH_DEC_LEN;
    HashNode* temp;
    temp = &g_session_all_dec[index];

    SessionData *session_data_temp;    // SESSION 链的数据节点
    time_t t = time(NULL);
    int curr_time = time(&t);

    printf("%s \n", session_str);

    while(temp->next != NULL){
        temp = temp->next; 
        if(strcmp(temp->key, session_str) == 0){
            
            session_data_temp = temp->value;            // 找到数据节点的 链头
            do{

                if(strcmp(session_data_temp->key, key) == 0){        // 数据名字相同
                    if(curr_time <= session_data_temp->time_store + session_data_temp->expire)  // 数据未过期
                        return session_data_temp->data;
                    else
                        return NULL;
                }

                printf("Key: %s Data: %s \n", session_data_temp->key, session_data_temp->data);
                // 搜索数据链中下一个数据的名字
                session_data_temp = session_data_temp->next;
            }while((session_data_temp != NULL ));

        }
    }
    
    return NULL;
}


/*  \bref  往已经存在的一个session里面添加sessiondata
    \param 传入 req   
    \return  成功返回 1, 失败返回非0 
    \return -2: session 链中没有找到目标 session_str 
    \return -1: req 对象没有 session  */
extern int SessionAddR(const Request* req, char* key, char* data) 
{
    char *str1;
	for(int i=0; i<=req->p_int; i++) {
		if(strcmp(req->params[i].key, "Cookie")==0) {
			str1 = strstr(req->params[i].data, "dmfsession=");
            *(str1 + 21) = '\0';
			if(str1 != NULL){
                return SessionAdd( str1+11, key, data);
			}else{
                return -1;   // req 对象没有 session
            }
		}
	}
    return -1;       // req 对象没有 session
}


/*  \brief 往已经存在的一个session里面添加sessiondata
    \return 成功返回 1, 失败返回 0    */
extern int SessionAdd(char* session_str, char* key, char* value) 
{
    
    SessionData* dataNode = (SessionData*)malloc(sizeof(SessionData));    // data node
    dataNode->key = (char*)malloc(sizeof(char)*strlen(key));       // malloc data name
    dataNode->data = (char*)malloc(sizeof(char)*strlen(value));    // malloc data 
    strcpy(dataNode->key, key);      // set data name
    strcpy(dataNode->data, value);   // set data
    time_t t = time(NULL);
    dataNode->time_store = time(&t);
    dataNode->expire = SESSION_EXPIRE_DEFAULT;
    dataNode->next = NULL;



    // 计算当前 session 序号是多少
    unsigned int index = BKDRHash(session_str) % HASH_DEC_LEN;
    HashNode* temp = &g_session_all_dec[index];

    while(temp->next != NULL){
        temp = temp->next; 
        if(strcmp(temp->key, session_str) == 0){
            
            if(temp->value == NULL){
                temp->value = dataNode;
                return 1;
            }else{
                dataNode->next = temp->value;
                temp->value = dataNode;
                return 1;
            }
        }
    }

    return -2;       // session链中 没有找到目标 session_str
}


/*  \brief 修改session data 
    \return 修改成功返回1  */
extern int UpdateSessionDataR(const Request* req, char* key, char* newdata) 
{
    char *str1;
	for(int i=0; i<=req->p_int; i++) {
		if(strcmp(req->params[i].key, "Cookie")==0) {
			str1 = strstr(req->params[i].data, "dmfsession=");
            *(str1 + 21) = '\0';
			if(str1 != NULL){
                return UpdateSessionData( str1+11, key, newdata);
			}else{
                return 0;   // Cookie 没有 session
            }
		}
	}
    return 0;       // req 对象没有 session
}


/*  \brief 修改session data 
    \return 修改成功返回1  */
extern int UpdateSessionData(char* session_str, char* key, char* newdata) 
{
    // 计算当前 session 序号是多少
    unsigned int index = BKDRHash(session_str) % HASH_DEC_LEN;
    HashNode* temp;
    temp = &g_session_all_dec[index];

    SessionData *session_data_temp;    // SESSION 链的数据节点
    time_t t = time(NULL);
    int curr_time = time(&t);

    while(temp->next != NULL){
        temp = temp->next; 
        if(strcmp(temp->key, session_str) == 0){
            
            session_data_temp = temp->value;            // 找到数据节点的 链头
            do{

                if(strcmp(session_data_temp->key, key) == 0){        // 数据名字相同

                    free(session_data_temp->data);
                    session_data_temp->data = (char*)malloc(sizeof(char)*strlen(newdata));
                    strcpy(session_data_temp->data, newdata);
                    session_data_temp->time_store = curr_time;
                    session_data_temp->expire = SESSION_EXPIRE_DEFAULT;
                    return 1;
                }

                printf("Key: %s Data: %s \n", session_data_temp->key, session_data_temp->data);
                // 搜索数据链中下一个数据的名字
                session_data_temp = session_data_temp->next;
            }while((session_data_temp != NULL ));

        }
    }
    return 0;
}


/* \brief 删除 Session data 
    \return  返回1 删除成功*/
extern int DeleteSessionData(char* session_str, char* key) 
{


    return 1;
}

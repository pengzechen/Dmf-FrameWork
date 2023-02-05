
#include "session.h"

extern int get_random_str(char* random_str, const int random_len)
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

// create a empty session file
extern int createSession(char * session_name) {
	
    // splicing the session path and session_name(filename)
    char path_name[64];
    memset(path_name, 0, 64);
    strcat(path_name, SESSION_DIR);
    strcat(path_name, session_name);

    FILE *fp;
	fp = fopen( path_name, "wb" );
	if(fp == NULL){ printf(" open Failed");  }
	for(int i=0; i < SIGNAL_SESSION_MAX_NUM; i++){
        SessionPart sp;
        memset(sp.name, 0, SESSION_NAME_LEN);
        sp.start = 0;
        sp.len = 0;
        sp.session_type = Sstring; // default c_string
        fwrite(sp, sizeof(SessionPart), 1, fp);
    }
	fclose(fp);

    return 0;
}

extern int sessionInit(){
    char random_str[11];
    memset(random_str, 0, 11);
    get_random_str(random_str, 10);

    createSession(random_str);
    return 0;

}
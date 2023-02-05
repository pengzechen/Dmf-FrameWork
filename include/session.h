#ifndef SESSION
#define SESSION


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

// the max number of key_values in each session file
#define SIGNAL_SESSION_MAX_NUM 64
#define SESSION_DIR "./session"

#define SESSION_NAME_LEN 64

typedef enum _SessionType {
    Sint = 1, Sstring, Sfloat
} SessionType;

typedef struct _SessionPart {
    char name[ SESSION_NAME_LEN ];
    SessionType session_type;
    unsigned int start;
    unsigned int len;
} SessionPart ;


extern int get_random_str(char* random_str, const int random_len);
extern int createSession(char * session_name);
extern int sessionInit();


#endif /* SESSION */
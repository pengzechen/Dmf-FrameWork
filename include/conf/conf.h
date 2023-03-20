#ifndef _H_CONF_

#define _H_CONF_

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string.h>


typedef struct conf_model {
    char host[64];
    int port;
    char username[32];
    char password[32];
    char database[32];
} conf_model;

typedef struct conf_server {
    int port;
} conf_server;

typedef struct Server_Conf {
    conf_model _conf_model;
    conf_server _conf_server;
} Server_Conf;

// 全局的配置变量
Server_Conf server_conf_all;

// #include <libxml/xpathInternals.h>

extern void test();
extern void ConfInit();


#endif // _H_CONF_
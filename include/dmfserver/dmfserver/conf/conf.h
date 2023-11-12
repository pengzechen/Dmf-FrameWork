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

#ifndef __CONF_INCLUDE__

#define __CONF_INCLUDE__

// #include <libxml/xpathInternals.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <string.h>

// server 模块

typedef enum _ServerMode {
    SimpleServer, 
    IOCPServer, 
    SSLServer,
#ifdef __linux__
    EpollServer,
#endif
} ServerMode;

typedef struct conf_server {
    int port;
    char host[64];
    ServerMode mode;
    char cert_private[128];
    char cert_public[128];
    
} conf_server;

// model 模块
typedef struct conf_model {
    char host[64];
    int port;
    char username[32];
    char password[32];
    char database[32];
} conf_model;


// Router 模块
typedef struct conf_router {
    char static_dir[1024];
} conf_router;


typedef struct server_cf_t {
    conf_model _conf_model;
    conf_server _conf_server;
    conf_router _conf_router;
} server_cf_t;


// conf 全局的配置变量
extern server_cf_t g_server_conf_all;

#ifdef __cplusplus
extern "C" {
#endif

extern void test();
extern void conf_init();

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __CONF_INCLUDE__
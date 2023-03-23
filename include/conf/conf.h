/* 
Copyright 2023 Ajax

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
*/

#ifndef _H_CONF_

#define _H_CONF_

// #include <libxml/xpathInternals.h>
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

    char cert_private[128];
    char cert_public[128];
    
} conf_server;

typedef struct Server_Conf {
    conf_model _conf_model;
    conf_server _conf_server;
} Server_Conf;


// conf 全局的配置变量
Server_Conf g_server_conf_all;


extern void test();
extern void ConfInit();


#endif // _H_CONF_
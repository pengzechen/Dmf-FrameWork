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

   
#include <dmfserver/conf/conf.h>

// conf 全局的配置变量
Server_Conf g_server_conf_all;


void conf_init()
{

    printf("[Conf: Info] start confguring...\n");

    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlReadFile("./conf.xml", "gbk", XML_PARSE_NOBLANKS);
    if (doc != NULL) {
        printf("[Conf: Info] conf.xml loaded!\n");
    }
    xmlNodePtr curNode;
    // cyg win
    // 得到根节点
    curNode = xmlDocGetRootElement(doc);
    if (curNode == NULL) {
        printf("[Conf: Warn] doc is empty \n");
        xmlFreeDoc(doc);
        return;
    }

    //  查看根节点名字是否是 dmfserver
    if (xmlStrcmp(curNode->name, BAD_CAST "dmfserver")) {
        printf("[Conf: Warn] root not dmfserver\n");
        xmlFreeDoc(doc);
        return;
    }

    xmlChar *szKey;
    xmlNodePtr child;
    child = curNode->children;      // curNode 是根节点
                                    // child 是模块层
    while (child != NULL){
        if (!xmlStrcmp(child->name, (const xmlChar *)"model")) {
            // szKey = xmlNodeGetContent(child);
            // printf("model: %s\n", szKey);
            // xmlFree(szKey);
            break;
        }
        child = child->next;
    }

    curNode = child->children; // child 是model节点
    while (curNode != NULL) {
        szKey = xmlNodeGetContent(curNode);
        if (!xmlStrcmp(curNode->name, (const xmlChar *)"host"))
            strcpy(g_server_conf_all._conf_model.host, szKey);
        else if (!xmlStrcmp(curNode->name, (const xmlChar *)"port"))
            g_server_conf_all._conf_model.port = atoi(szKey);
        else if (!xmlStrcmp(curNode->name, (const xmlChar *)"username"))
            strcpy(g_server_conf_all._conf_model.username, szKey);
        else if (!xmlStrcmp(curNode->name, (const xmlChar *)"password"))
            strcpy(g_server_conf_all._conf_model.password, szKey);
        else if (!xmlStrcmp(curNode->name, (const xmlChar *)"database"))
            strcpy(g_server_conf_all._conf_model.database, szKey);
        else continue;
        curNode = curNode->next;
    }
    xmlFree(szKey);

    xmlFreeDoc(doc);


    g_server_conf_all._conf_server.port = 80;
    strcpy(g_server_conf_all._conf_server.host, "localhost");
    strcpy(g_server_conf_all._conf_server.cert_public, "./cert/localhost.pem");
    strcpy(g_server_conf_all._conf_server.cert_private, "./cert/localhost-key.pem");
    g_server_conf_all._conf_server.mode = SimpleServer;

    strcpy(g_server_conf_all._conf_router.static_dir, "static");

    printf("[Conf: Info] conf init successfully...\n");
}

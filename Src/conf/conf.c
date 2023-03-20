
#include <conf/conf.h>

void ConfInit()
{
    xmlKeepBlanksDefault(0);
    xmlDocPtr doc = xmlReadFile("./conf.xml", "gbk", XML_PARSE_NOBLANKS);
    if (doc != NULL)
    {
        printf("[info] conf.xml load ok!\n");
    }
    xmlNodePtr curNode;
    // 得到根节点
    curNode = xmlDocGetRootElement(doc);
    if (curNode == NULL)
    {
        printf("[warn] doc is empty \n");
        xmlFreeDoc(doc);
        return;
    }

    //  查看根节点名字是否是 dmfserver
    if (xmlStrcmp(curNode->name, BAD_CAST "dmfserver"))
    {
        printf("[warn] root not dmfserver\n");
        xmlFreeDoc(doc);
        return;
    }

    xmlChar *szKey;
    xmlNodePtr child;
    child = curNode->children; // curNode 是根节点
    while (child != NULL)
    {
        if (!xmlStrcmp(child->name, (const xmlChar *)"model"))
        {
            // szKey = xmlNodeGetContent(child);
            // printf("model: %s\n", szKey);
            // xmlFree(szKey);
            break;
        }
        child = child->next;
    }

    curNode = child->children; // child 是model节点
    while (curNode != NULL)
    {
        szKey = xmlNodeGetContent(curNode);
        if (!xmlStrcmp(curNode->name, (const xmlChar *)"host"))
        {
            strcpy(server_conf_all._conf_model.host, szKey);
        }
        if (!xmlStrcmp(curNode->name, (const xmlChar *)"port"))
        {
            server_conf_all._conf_model.port = atoi(szKey);
        }
        if (!xmlStrcmp(curNode->name, (const xmlChar *)"username"))
        {
            strcpy(server_conf_all._conf_model.username, szKey);
        }
        if (!xmlStrcmp(curNode->name, (const xmlChar *)"password"))
        {
            strcpy(server_conf_all._conf_model.password, szKey);
        }
        if (!xmlStrcmp(curNode->name, (const xmlChar *)"database"))
        {
            strcpy(server_conf_all._conf_model.database, szKey);
        }
        curNode = curNode->next;
    }
    xmlFree(szKey);

    xmlFreeDoc(doc);

    printf("[Server: Info] conf init successfully...\n");
}

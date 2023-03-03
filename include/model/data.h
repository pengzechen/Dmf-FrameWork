

#ifndef DATA_STRUCT
#define DATA_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ObjectNode ObjectNode;
typedef union NodeData NodeData;
typedef enum NodeDataType NodeDataType;

union NodeData {
	char* d_str;
	int* d_int;
	
};

enum NodeDataType {
	D_CHAR = 1,
	D_INT,
	D_NODE
};

struct ObjectNode {
	char NodeName[32];
	NodeDataType ndt;
	NodeData nd;

	ObjectNode* mnChild;
	ObjectNode* mnBro;
	
};

extern ObjectNode* CreateObjectNode(char* nodeName, NodeDataType ndt, void* anyData);

extern ObjectNode* CreateRootNode(char* nodeName, NodeDataType ndt, void* anyData);

extern void AppendBro(ObjectNode* mn, ObjectNode* on);

extern void AppendChild(ObjectNode* mn, ObjectNode* child);

extern void ShowNodeData(ObjectNode* mn);

#endif
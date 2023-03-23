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
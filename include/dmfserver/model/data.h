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

typedef struct objectNode objectNode;
typedef objectNode* objPtr;

struct objectNode {
	char NodeName[64];
	void* data;
	int size;
	objPtr mnChild;
	objPtr mnBro;
	int type;
};


extern objPtr CreateObjectNode(char* nodeName, char* data);

extern void AppendBro(objPtr mn, objPtr bro);

extern void AppendChild(objPtr mn, objPtr child);

extern void ShowNodeData(objPtr mn);

#endif
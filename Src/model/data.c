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
#include <model/data.h>

// create a new modle node with NodeName
ObjectNode* CreateObjectNode(char* nodeName, NodeDataType ndt, void* anyData)
{
	ObjectNode* mn = (ObjectNode*)malloc(sizeof(ObjectNode));
	if(mn == NULL) 
	{
		printf("Create error \n");
		return NULL;
	}
	strcpy(mn->NodeName, nodeName);

	mn->mnBro = NULL;
	mn->mnChild = NULL;
	mn->ndt = ndt;
	switch(ndt){
		case D_CHAR:
			mn->nd.d_str = (char*)malloc(sizeof(char)*strlen((char*)anyData));
			memset(mn->nd.d_str, 0, sizeof(char)*strlen((char*)anyData));
			strcpy(mn->nd.d_str, (char*)anyData);
			break;
		case D_INT:
			mn->nd.d_int = (int*)malloc(sizeof(int));
			mn->nd.d_int = (int*)anyData;
			break;
		default:
			// No such type
			break;
	}
	return mn;
}

// create a new modle node with NodeName
ObjectNode* CreateRootNode(char* nodeName, NodeDataType ndt, void* anyData)
{
	ObjectNode* mn = (ObjectNode*)malloc(sizeof(ObjectNode));
	if(mn == NULL) 
	{
		printf("Create error \n");
		return NULL;
	}
	strcpy(mn->NodeName, nodeName);

	mn->mnBro = NULL;
	mn->mnChild = NULL;
	mn->ndt = ndt;
	
	return mn;
}

// append a Bro to a ObjectNode
void AppendBro(ObjectNode* mn, ObjectNode* on)
{
	if(mn->mnBro == NULL){
		mn->mnBro = on;
	}else{
		on->mnBro = mn->mnBro;
		mn->mnBro = on;
	}

	//return mn;
}

// append a child to a ObjectNode
void AppendChild(ObjectNode* mn, ObjectNode* child)
{
	if(mn->mnChild != NULL)
		return;
	mn->mnChild = child;
}

void ShowNodeData(ObjectNode* mn)
{
	if(mn == NULL) return;
	switch(mn->ndt){
		case D_CHAR:
			printf("%s: %s \n", mn->NodeName, mn->nd.d_str);
			break;
		case D_INT:
			printf("%s: %d \n", mn->NodeName, mn->nd.d_int);
			break;
		case D_NODE:
			printf("%s: Object \n", mn->NodeName);
			break;
		default:
			// No such type
			return;
	}
}

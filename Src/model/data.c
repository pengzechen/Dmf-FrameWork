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

#include <dmfserver/model/data.h>

// create a new modle node with NodeName
extern objPtr CreateObjectNode(char* nodeName, char* data)
{
	objPtr mn = (objPtr)malloc(sizeof(objectNode));
	if(mn == NULL){
		printf("Create error \n");
		return NULL;
	}
	strcpy(mn->NodeName, nodeName);
	mn->data = malloc(sizeof(data)* strlen(data));
	strcpy(mn->data, data);
	mn->mnBro = NULL;
	mn->mnChild = NULL;
	return mn;
}


// append a Bro to a objectNode
void AppendBro(objPtr mn, objPtr bro)
{
	if(mn->mnBro == NULL){
		mn->mnBro = bro;
	}else{
		bro->mnBro = mn->mnBro;
		mn->mnBro = bro;
	}
	//return mn;
}

// append a child to a objectNode
void AppendChild(objPtr mn, objPtr child)  // 孩子是有序的 不能从链头加
{
	if(mn->mnChild != NULL)
		return;
	mn->mnChild = child;
}

void ShowNodeData(objPtr mn)
{
	objPtr p = mn;
	
	do{
		printf("%s ", (char*)p->data);
		p = p->mnBro;
	}while(p != NULL);
	
}

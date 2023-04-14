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
#ifndef TEMPLATE
#define TEMPLATE


#define TEMPLATE_RESULT_SIZE 4096 
#define DEC_NUM 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Kvmap {
	int type;
	char* key;
	char* value;					// #	1
	
	char* dec[ DEC_NUM ];
	void (*Func)(char*, char *);			// 
};

#ifdef __cplusplus
extern "C" {
#endif

	char * local_template(char * template_path);

	static void parse_dec(char* tt, char* dec[], char* inner);

	char * parse_context(char *context, struct Kvmap *kv, int kv_num);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif
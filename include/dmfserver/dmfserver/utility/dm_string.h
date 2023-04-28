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


#ifndef __DM_STRING_INCLUDE__
#define __DM_STRING_INCLUDE__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//定义 String 结构体
typedef struct {
    char* str;
    int len;
} dm_string;


#ifdef __cplusplus
extern "C" {
#endif

	dm_string* 	dm_str(char* s);
	void 		dm_str_free(dm_string* s);
	dm_string* 	dm_str_cat(dm_string* s1, dm_string* s2);
	int 		dm_str_cmp(dm_string* s1, dm_string* s2);
	dm_string* 	dm_str_rev(dm_string* s);
	dm_string* 	dm_str_up(dm_string* s);
	dm_string* 	dm_str_low(dm_string* s);
	int 		dm_str_find_sub(dm_string* s, char* sub);
	dm_string* 	dm_str_sub(dm_string* s, int start, int end);
	int 		dm_str_toint(dm_string* s);
	dm_string* 	dm_str_tostr(int num);
	dm_string* 	dm_str_trim(dm_string* s);
	int 		dm_str_start_with(dm_string* s, char* prefix);
	int 		dm_str_end_with(dm_string* s, char* suffix);
	dm_string* 	dm_str_replace(dm_string* s, char* oldStr, char* newStr);
	int 		dm_index_char(dm_string* s, char c);
	void 		print_str(dm_string *str);



#ifdef __cplusplus
}           /* end of the 'extern "C"' block */
#endif

#endif // __DM_STRING_INCLUDE__
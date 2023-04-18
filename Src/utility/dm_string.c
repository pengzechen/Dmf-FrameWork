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

#include <dmfserver/utility/dm_string.h>

//创建一个新的字符串
dm_string* dm_str(char* s) {
    dm_string* newString = (dm_string*)malloc(sizeof(dm_string));
    newString->len = strlen(s);
    newString->str = (char*)malloc(sizeof(char) * (newString->len + 1));
    strcpy(newString->str, s);
    return newString;
}

//释放字符串所占用的内存
void dm_str_free(dm_string* s) {
    free(s->str);
    free(s);
}

//将两个字符串拼接起来
dm_string* dm_str_cat(dm_string* s1, dm_string* s2) {
    dm_string* newString = (dm_string*)malloc(sizeof(dm_string));
    newString->len = s1->len + s2->len;
    newString->str = (char*)malloc(sizeof(char) * (newString->len + 1));
    strcpy(newString->str, s1->str);
    strcat(newString->str, s2->str);
    return newString;
}

//比较两个字符串是否相等
int dm_str_cmp(dm_string* s1, dm_string* s2) {
    if (s1->len != s2->len) {
        return 0;
    }
    return strcmp(s1->str, s2->str) == 0;
}

//将字符串反转
dm_string* dm_str_rev(dm_string* s) {
    dm_string* newString = (dm_string*)malloc(sizeof(dm_string));
    newString->len = s->len;
    newString->str = (char*)malloc(sizeof(char) * (newString->len + 1));
    for (int i = 0; i < s->len; i++) {
        newString->str[i] = s->str[s->len - i - 1];
    }
    newString->str[newString->len] = '\0';
    return newString;
}

//将字符串转换为大写
dm_string* dm_str_up(dm_string* s) {
    dm_string* newString = (dm_string*)malloc(sizeof(dm_string));
    newString->len = s->len;
    newString->str = (char*)malloc(sizeof(char) * (newString->len + 1));
    for (int i = 0; i < s->len; i++) {
        if (s->str[i] >= 'a' && s->str[i] <= 'z') {
            newString->str[i] = s->str[i] - 32;
        } else {
            newString->str[i] = s->str[i];
        }
    }
    newString->str[newString->len] = '\0';
    return newString;
}

//将字符串转换为小写
dm_string* dm_str_low(dm_string* s) {
    dm_string* newString = (dm_string*)malloc(sizeof(dm_string));
    newString->len = s->len;
    newString->str = (char*)malloc(sizeof(char) * (newString->len + 1));
    for (int i = 0; i < s->len; i++) {
        if (s->str[i] >= 'A' && s->str[i] <='Z') {
			newString->str[i] = s->str[i] + 32;
		} else {
			newString->str[i] = s->str[i];
		}
	}
	newString->str[newString->len] = '\0';
	return newString;
}

//查找字符串中是否包含子串
int dm_str_find_sub(dm_string* s, char* sub) {
	char* ptr = strstr(s->str, sub);
	if (ptr == NULL) {
		return -1;
	} else {
		return ptr - s->str;
	}
}

//截取字符串
dm_string* dm_str_sub(dm_string* s, int start, int end) {
	dm_string* newString = (dm_string*)malloc(sizeof(dm_string));
	newString->len = end - start + 1;
	newString->str = (char*)malloc(sizeof(char) * (newString->len + 1));
	for (int i = start; i <= end; i++) {
		newString->str[i - start] = s->str[i];
	}
	newString->str[newString->len] = '\0';
	return newString;
}

//字符串转数字
int dm_str_toint(dm_string* s) {
    int result = 0;
    int sign = 1;
    int i = 0;
    if (s->str[0] == '-') {
        sign = -1;
        i++;
    }
    for (; i < s->len; i++) {
        if (s->str[i] >= '0' && s->str[i] <= '9') {
            result = result * 10 + (s->str[i] - '0');
        } else {
            return 0;
        }
    }
    return sign * result;
}

//数字转字符串
dm_string* dm_str_tostr(int num) {
    char buffer[20];
    sprintf(buffer, "%d", num);
    return dm_str(buffer);
}

//去掉字符串首尾空格
dm_string* dm_str_trim(dm_string* s) {
    int left = 0, right = s->len - 1;
    while (left <= right && s->str[left] == ' ') {
        left++;
    }
    while (left <= right && s->str[right] == ' ') {
        right--;
    }
    return dm_str_sub(s, left, right);
}

//判断字符串是否以某个子串开头
int dm_str_start_with(dm_string* s, char* prefix) {
    if (strlen(prefix) > s->len) {
        return 0;
    }
    return strncmp(s->str, prefix, strlen(prefix)) == 0;
}

//判断字符串是否以某个子串结尾
int dm_str_end_with(dm_string* s, char* suffix) {
    int suffixLen = strlen(suffix);
    if (suffixLen > s->len) {
        return 0;
    }
    return strncmp(s->str + s->len - suffixLen, suffix, suffixLen) == 0;
}

//替换字符串中的某个子串
dm_string* dm_str_replace(dm_string* s, char* oldStr, char* newStr) {
    int oldLen = strlen(oldStr), newLen = strlen(newStr);
    dm_string* result = dm_str(s->str);
    char* ptr = strstr(result->str, oldStr);
    while (ptr != NULL) {
        memmove(ptr + newLen, ptr + oldLen, strlen(ptr + oldLen) + 1);
        memcpy(ptr, newStr, newLen);
        result->len += newLen - oldLen;
        ptr = strstr(ptr + newLen, oldStr);
    }
    return result;
}

//在字符串中查找某个字符
int dm_index_char(dm_string* s, char c) {
    char* ptr = strchr(s->str, c);
    if (ptr == NULL) {
        return -1;
    } else {
        return ptr - s->str;
    }
}

//debug 输出字符串
void print_str(dm_string *str) {
    printf("\n%s %d\n", str->str, str->len);
}


int test_string() {

    print_str(dm_str("string tests"));

    dm_string* str2_res = dm_str_cat( dm_str("string tests1"), dm_str("string tests2") );
    print_str(str2_res);

    dm_string* str3_res = dm_str_rev(dm_str("abcdefg"));
    print_str(str3_res);

    dm_string* str4_res = dm_str_up(dm_str("abcdefg"));
    print_str(str4_res);

    dm_string* str5_res = dm_str_low(dm_str("ABCDEFG"));
    print_str(str5_res);
    
    printf("\n%d\n", dm_index_char(dm_str("ABCDEFG"), 'C'));

    
   
    return 0;
}

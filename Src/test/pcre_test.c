#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <jansson.h>
#include <pcre.h>
#elif __WIN32__
#include <jansson/jansson.h>
#include <pcre/pcre.h>
#endif
#define OVECCOUNT 30 /* should be a multiple of 3 */

int pcre_test() 
{
	pcre  *re;
    const char *error;
    int  erroffset;
    int  ovector[OVECCOUNT];
    int  rc, i;
    char  src [] = "111 <title>Hello World</title> 222";   // 要被用来匹配的字符串
    char  pattern [] = "<title>(.*)</(tit)le>";            // 将要被编译的字符串形式的正则表达式
    printf("String : %s\n", src);
    printf("Pattern: \"%s\"\n", pattern);
    re = pcre_compile(pattern,      // pattern, 输入参数，将要被编译的字符串形式的正则表达式
                      0,            // options, 输入参数，用来指定编译时的一些选项
                      &error,       // errptr, 输出参数，用来输出错误信息
                      &erroffset,   // erroffset, 输出参数，pattern中出错位置的偏移量
                      NULL);        // tableptr, 输入参数，用来指定字符表，一般情况用NULL
    // 返回值：被编译好的正则表达式的pcre内部表示结构
    if (re == NULL) {                 //如果编译失败，返回错误信息
        printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return 1;
    }
    rc = pcre_exec(re,            // code, 输入参数，用pcre_compile编译好的正则表达结构的指针
                   NULL,          // extra, 输入参数，用来向pcre_exec传一些额外的数据信息的结构的指针
                   src,           // subject, 输入参数，要被用来匹配的字符串
                   strlen(src),  // length, 输入参数， 要被用来匹配的字符串的指针
                   0,             // startoffset, 输入参数，用来指定subject从什么位置开始被匹配的偏移量
                   0,             // options, 输入参数， 用来指定匹配过程中的一些选项
                   ovector,       // ovector, 输出参数，用来返回匹配位置偏移量的数组
                   OVECCOUNT);    // ovecsize, 输入参数， 用来返回匹配位置偏移量的数组的最大大小
    // 返回值：匹配成功返回非负数，没有匹配返回负数
    if (rc < 0) {                     //如果没有匹配，返回错误信息
        if (rc == PCRE_ERROR_NOMATCH)
            printf("Sorry, no match ...\n");
        else
            printf("Matching error %d\n", rc);
        pcre_free(re);
        return 1;
    }
    printf("\nOK, has matched ...\n\n");   //没有出错，已经匹配
    for (i = 0; i < rc; i++) {             //分别取出捕获分组 $0整个正则公式 $1第一个()
        char *substring_start = src + ovector[2*i];
        int substring_length = ovector[2*i+1] - ovector[2*i];
 
        printf("$%2d: %.*s\n", i, substring_length, substring_start);
    }
 
    pcre_free(re);                     // 编译正则表达式re 释放内存
}

void jannson_test()
{
	json_t val;
    int type = json_typeof(&val);
    bool Judge = json_is_object(&val);
}

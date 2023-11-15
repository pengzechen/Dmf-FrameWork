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


#ifndef __EEQUEST_INCLDUE__
#define __EEQUEST_INCLDUE__


// #define MULTI_DEBUG
// #define REQUEST_DEBUG


#define MULTI_PART_MAX 		1024*1024	// multipart 数据大小
#define MULTI_PART_MAX_NUM 	20			// 最大multipart 数量


//******************  HTTP协议相关 *****************
// 第一行
#define HTTP_METHOD_MAX    5
#define HTTP_URI_MAX       512
#define HTTP_PROTOCOL_MAX  5
#define HTTP_VERSION_MAX   4

#define HTTP_BODY_MAX		 	1024*1024	// body 数据大小
//******************  HTTP协议相关 *****************

//******************  HTTP解析状态机 *****************

typedef  enum _HTTP_PARSE_STATE {
	PARSE_INVALID	,
	PARSE_START  	,
	PARSE_METHOD 	,

	PARSE_PATH_START      ,
	PARSE_PATH         ,

	PARSE_QUERY_START  ,
	PARSE_QUERY        ,
	PARSE_QUERY_TEMP   ,

	PARSE_PV_START     ,
	PARSE_PV_TEMP      ,
	PARSE_PV           ,

	PARSE_PARAM_START  ,
	PARSE_PARAM1		   ,
	PARSE_PARAM2		   ,
	PARSE_PARAM3		   ,
	
	PARSE_BODY_START  ,
	PARSE_BODY

} HTTP_PARSE_STATE ;

//******************  HTTP解析状态机 *****************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>

#include <dmfserver/utility/dm_map.h>


struct Multi_kv {
	char 				key[32];
	char 				data[64];
};

struct Multipart {
	struct Multi_kv 	name;
	struct Multi_kv 	dis;
	struct Multi_kv 	filename;
	char 		*		data;
	int 				length;
};

struct http_body_t {
	char 	*	body;
	size_t 		length;
};


struct req {
	char 			method		[ HTTP_METHOD_MAX ];
	char 			path		[ HTTP_URI_MAX];
	char 			protocol	[ HTTP_PROTOCOL_MAX ];
	char 			version		[ HTTP_VERSION_MAX ];
	hashmap_tp     query;
	hashmap_tp     params;
	struct http_body_t 		body;

	int 			multi_part_num;
	struct Multipart * multi[ MULTI_PART_MAX_NUM ];
};

typedef struct req request_t;

#ifdef __cplusplus
extern "C" {
#endif

void req_parse_multi_part (request_t * request, char * boundary );

void req_parse_init(request_t * request);

void req_parse_http(request_t * request, char * data);

void req_get_session_str(const request_t * req,  char session_str[]);

void req_get_param(const request_t * req, char * key, 	char data[]);

void req_get_query(const request_t * req, char * key, 	char data[]);

void req_get_ws_key(const request_t * req,  char ws_key[]);

void req_free(request_t * req);


#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __EEQUEST_INCLDUE__
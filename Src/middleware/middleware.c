
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
   
#include <dmfserver/middleware/middleware.h>
#include <dmfserver/request.h>
// #include "jwt_token.c"


extern void middleware_init()
{
    printf("[Middleware: Info] middleware init successfully!\n");
    printf("\n");
}

extern void middleware_handle(const Request* req)
{
    char res[64] = {0};
    req_get_param(req, "Host", res);
    printf("    [MiddleWare: ] Host: %s\n", res);
}
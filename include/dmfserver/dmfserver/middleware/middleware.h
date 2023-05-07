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


#ifndef __MIDDLEWARE_INCLUDE__
#define __MIDDLEWARE_INCLUDE__

#include <dmfserver/request.h>
#include <dmfserver/response.h>

#ifdef __cplusplus
extern "C" {
#endif

    static int middleware_check_if_too_often(const Request* req);

    extern void middleware_init();

    extern int middleware_handle(const Request* req);

#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif


#endif // __MIDDLEWARE_INCLUDE__
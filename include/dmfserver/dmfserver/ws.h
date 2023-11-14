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

#ifndef __WS_INCLUDE__
#define __WS_INCLUDE__


#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <dmfserver/connection.h>


#ifdef __WIN32__
#include <WinSock2.h>		// 为了使用 send
#elif __linux__
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>         // for close
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void upto_ws_prot( connection_tp conn, char key[]) ;


#ifdef __cplusplus
}		/* end of the 'extern "C"' block */
#endif

#endif // __WS_INCLUDE__
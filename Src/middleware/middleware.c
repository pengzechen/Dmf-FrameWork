
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
#include <dmfserver/mdb/mdb_operate.h>

#ifdef __WIN32__
	#include <WinSock2.h>
	#include <WS2tcpip.h>
#endif // __win32__

extern void middleware_init()
{
    printf("[Middleware: Info] middleware init successfully!\n");
    printf("\n");
}

static int middleware_check_if_too_often(const Request* req)
{
    // 得到客户端ip地址
    struct sockaddr_in client_addr;
    memset(&client_addr, 0x00, sizeof(client_addr));
    socklen_t nl=sizeof(client_addr);
    getpeername(req->pfd.fd, (struct sockaddr*)&client_addr, &nl);
    char* addr=inet_ntoa(client_addr.sin_addr);  

    switch ( ip_check_valid(addr) ) {
        case 0:
            return 0;
        case 2:
            // printf("[Warn]%s request too often! \n", addr);
            return -2;
        case 3:
            // printf("[Warn]%s has been ban! \n", addr);
            return -3;
        default: return 0;
    }
}

// 返回值大于零说明所有中间件都经过检测
// 返回值小于零则是相应的中间件未通过
// 在返回之前要调用response模块返回请求错误
extern int middleware_handle(const Request* req)
{

    if( middleware_check_if_too_often(req) != 0 ) {

        Res_without_permission(req->pfd.fd);
        return -1001;
    }

    return 1;
}
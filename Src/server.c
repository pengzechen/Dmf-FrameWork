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
   
#include <dmfserver/server.h>
#include <dmfserver/connection.h>
#include <dmfserver/cfg.h>


static SSL_CTX* get_ssl_ctx()
{
    SSL_CTX *ctx ;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    if (SSL_CTX_use_certificate_file(ctx, g_server_conf_all._conf_server.cert_public , SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, g_server_conf_all._conf_server.cert_private , SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    return ctx;
}


static void req_res_handler(int acceptFd ) 
{
	
	char res_str[ RECEIVE_MAX_BYTES ] = {'\0'};

	Request req1;
	int receive_bytes = recv( acceptFd, res_str, sizeof(res_str), 0 );
    printf("recv: \n%s\n", res_str);

    Perfd pfd = {.fd = acceptFd, .ssl = NULL};
    req_parse_init(&req1);
	req_parse_http(&req1, res_str);
	
	char time [30] = {'\0'};
	server_time(time);
	printf("[%s][Server: Info] %s\n", time, req1.path);
	
	router_handle(acceptFd, &req1);
	//通过请求的 path 调用了对应的处理函数
	
	req_free(&req1);

}


void simple_server_make()
{
#ifdef __WIN32__
    wsa_init();
#endif 
    int sListen = create_socket();

    struct sockaddr_in sock_in;
	int sock_in_len = sizeof(sock_in);
    int sAccept;
    
	while(1){
		sAccept = accept(sListen, (struct sockaddr *)&sock_in, &sock_in_len);
		req_res_handler( sAccept );
	}
	
    close(sListen); //关闭 socket
    
#ifdef __WIN32__
    wsa_cleanup();
#endif
}


void simple_ssl_server_make()
{
	SSL_CTX *ctx = get_ssl_ctx();
#ifdef __WIN32__
    wsa_init();
#endif
    int sListen = create_socket();

    struct sockaddr_in sock_in;
	int sock_in_len = sizeof(sock_in);

    SSL *ssl;
    char buf[MAXBUF + 1]; 
    int sAccept;
    int len;
    while (1) {
        
        sAccept = accept(sListen, (struct sockaddr *)&sock_in, &sock_in_len);

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sAccept);
        int ret = SSL_accept(ssl);
        printf("error code: %d %d\n", SSL_get_error(ssl, ret), ret);

        memset(buf, 0, MAXBUF + 1);
        len = SSL_read(ssl, buf, MAXBUF);
        // printf("%s\n", buf);
        char res[128] = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nServer:Dmfserver\r\n\r\n OK OK";
        len = SSL_write(ssl, res, strlen(res));
        
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sAccept);
    }

    close(sListen);
#ifdef __WIN32__
    wsa_cleanup();
#endif
    SSL_CTX_free(ctx);
}


#ifdef __WIN32__  // WIN32

_Atomic int all = 0;

DWORD WINAPI iocp_handle_io(LPVOID lpParam)
{
    
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    per_handle_data_tp PerHandleData;
    per_io_data_tp PerIoData;
    

    while(1){

        if(0 == GetQueuedCompletionStatus(CompletionPort, 
            &BytesTransferred, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE))
        {
            if( (GetLastError() ==WAIT_TIMEOUT)) { //|| (GetLastError() == ERROR_NETNAME_DELETED ) ){
                printf("closingsocket %d\n", PerHandleData->Socket); 
                closesocket(PerHandleData->Socket);
                connection_tp conn;
                conn->per_handle_data = PerHandleData;
                conn->per_io_data = PerIoData;
                connection_free_base(conn);
                continue;
            }else{
                OutErr("GetQueuedCompletionStatus failed!");
            }
            return 0;
        } 

        connection_tp conn_ptr = PerHandleData->conn;
           
        // 说明客户端已经退出
        if(BytesTransferred == 0) {
            printf("closingsocket %d\n", PerHandleData->Socket);
            connection_close(conn_ptr);
            free(conn_ptr->req);
#ifdef __SERVER_MPOOL__
            pool_free( conn_ptr->per_io_data );
            pool_free2( conn_ptr->per_handle_data );
#else 
            free( conn_ptr->per_io_data );
            free( conn_ptr->per_handle_data );
#endif // __SERVER_MPOOL__
            free(conn_ptr);
            continue;
        }
        
#if 0
        // debug 使用
        // connection_tp conn_ptr = PerHandleData->conn;

        req_parse_init(conn_ptr->req);
        // 解析 http 请求
        req_parse_http(conn_ptr->req, conn_ptr->per_io_data->Buffer);

        char* res_str = "HTTP/1.1 200\r\n\r\nhello world!";
        send(PerHandleData->Socket, res_str, strlen(res_str), 0);
        closesocket(PerHandleData->Socket);

        req_free(conn_ptr->req);
        free(conn_ptr->req);

    #ifdef __SERVER_MPOOL__
        pool_free( conn_ptr->per_io_data );
        pool_free2( conn_ptr->per_handle_data );
    #else 
        free( conn_ptr->per_io_data );
        free( conn_ptr->per_handle_data );
    #endif // __SERVER_MPOOL__
        free(conn_ptr);

#else
        
        // 初始化一个 request
        req_parse_init(conn_ptr->req);
        // 解析 http 请求
        req_parse_http(conn_ptr->req, conn_ptr->per_io_data->Buffer);
        
        // 根据解析出来的结果运行中间件
        // if( middleware_handle(conn_ptr->req) < 0) {
        //     connection_close(conn_ptr);
        //     connection_free(conn_ptr);
        //     continue;
        // }

        // 进行必要日志记录
        char time [30] = {'\0'};
        server_time(time);
        log_info("SERVER", 211, "[%s][Server: Info] %s %d id: %d , all: %d", 
                time , conn_ptr->req->path, strlen(conn_ptr->per_io_data->Buffer), 
                GetCurrentThreadId (), all++);
        memset(time, 0, 30);
        
        /*   
            *req 对象进入路由模块
            *路由模块调用用户的view函数
            *在view函数中必须调用response模块进行返回
            */
        router_handle(conn_ptr->per_handle_data->Socket, conn_ptr->req);
        
        connection_free(conn_ptr);
#endif
    }

    return 0;
}


int iocp_server_make() 
{

    wsa_init();
    
    HANDLE completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    SYSTEM_INFO _system_info;
    GetSystemInfo(&_system_info);

    for(int i = 0; i < _system_info.dwNumberOfProcessors * 2; i++) {
        HANDLE hProcessIO = CreateThread(NULL, 0, iocp_handle_io, completion_port, 0, NULL);
        if(hProcessIO) 
            CloseHandle(hProcessIO);
    }

    
    // if configure not define port then use SERVER_PORT
    int serverPort;
    if(g_server_conf_all._conf_server.port == 0)
    serverPort = SERVER_PORT;
    else serverPort = g_server_conf_all._conf_server.port;
    // Listening socket
    SOCKET sListen = iocp_bind_socket(serverPort);

    SOCKET sClient;

    // per_handle_data_t 指针
    per_handle_data_tp PerHandleData;
    per_io_data_tp PerIoData;

    for(;;) {
        
        // wait for client
        sClient = WSAAccept(sListen, NULL, NULL, NULL, 0);
        //sClient = accept(sListen, 0, 0);

        connection_tp conn_ptr = new_connection();
        PerHandleData = conn_ptr->per_handle_data;
        PerIoData = conn_ptr->per_io_data;

        PerHandleData->Socket = sClient;
        PerHandleData->conn   = conn_ptr;
        

        // printf("per_handle_data_t size: %d\n", sizeof(per_handle_data_t));
        // printf("per_io_data_t size: %d\n", sizeof(per_io_data_t));
        // 将接入的客户端和完成端口联系起来
        CreateIoCompletionPort((HANDLE)sClient, completion_port,(DWORD)PerHandleData, 0);

        ZeroMemory( PerIoData, sizeof(per_io_data_t));
        PerIoData->DataBuf.buf = PerIoData->Buffer;
        PerIoData->DataBuf.len = DATA_BUFSIZE;

        // 投递一个WSARecv操作
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        WSARecv(sClient, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
    }
    

    DWORD dwByteTrans;
    //将一个已经完成的IO通知添加到IO完成端口的队列中.
    //提供了与线程池中的所有线程通信的方式.
    PostQueuedCompletionStatus(completion_port,dwByteTrans, 0, 0);  //IO操作完成时接收的字节数.

    closesocket(sListen);

    wsa_cleanup();

    printf("[MAIN: %d] exit\n", GetCurrentThreadId());
    
    return 0;
}


#elif __linux__ 


static void* epoll_handle_io(void* p)
{	
    thread_arg* arg = (thread_arg*)p;
    long i_listenfd = arg->fd;
    i_listenfd = createSocket();

    struct epoll_event ev, events[100];
    int epfd, nCounts;
    int i_connfd;
    epfd = epoll_create(100);

    // ev.events = EPOLLIN | EPOLLEXCLUSIVE;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = i_listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, i_listenfd, &ev);

    Request req1;
    char time [30] = {'\0'};
    char res_str[RECEIVE_MAX_BYTES] = {'\0'};
    int receive_bytes;

    for(;;)
    {
        nCounts = epoll_wait(epfd, events, 100, -1);
        for(int i = 0; i < nCounts; i++)
        {
            int tmp_epoll_recv_fd = events[i].data.fd;
            if(tmp_epoll_recv_fd == i_listenfd)	{
                i_connfd = accept(i_listenfd, (struct sockaddr*)NULL, NULL);	
                ev.events = EPOLLIN;
                ev.data.fd = i_connfd;
                epoll_ctl( epfd, EPOLL_CTL_ADD, i_connfd, &ev );
            } else {
                
                
                receive_bytes = recv( tmp_epoll_recv_fd, res_str, sizeof(res_str), 0 );
                Perfd pfd;
                pfd.fd = tmp_epoll_recv_fd;
                pfd.ssl = NULL;
                
                req_parse_http(&req1, res_str, pfd);
                server_time(time);

                log_info("SERVER", 506, "[%s][Server: Info] %s %d id: %d\n",time , req1.path, (int)strlen(res_str), getpid());
                
                memset(time, 0, 30);
                
                router_handle(tmp_epoll_recv_fd, &req1);
                
                req_free(&req1);
                // send(tmp_epoll_recv_fd, "HTTP/1.1 200 OK\r\n\r\nhello", 24, 0);


                //printf("[dmfServer Thread]\n");
                epoll_ctl(epfd, EPOLL_CTL_DEL, tmp_epoll_recv_fd, NULL);
                close(tmp_epoll_recv_fd);
            }
        }
    }
    close(i_listenfd);
    close(epfd);
}


extern void epoll_server_make() {

    long fd = 1;
    thread_arg* arg = malloc(sizeof(thread_arg));
    arg->cmp = g_cmp;
    arg->fd = fd;

    for (int i = 0; i < 1; ++i) {
        pthread_t roundCheck;
        pthread_create(&roundCheck, NULL, epoll_handle_io, (void*)arg);
        pthread_join(roundCheck, NULL);
    }
    
    return;
}


extern int epoll_ssl_server()
    {
        /*初始化socket*/
        int srvFd = create_socket_reuse();

        /*创建ssl上下文*/
        SSL_CTX *ctx = get_ssl_ctx();
        if(ctx == NULL){
            return 1;
        }

        printf("ssl load ok\n");
        
        int efd = epoll_create(100);
        assert(efd > 0);
        printf("epoll fd %d\n", efd);

        struct epoll_event events[100];
        struct epoll_event ev;
        ev.data.fd = srvFd;
        ev.events = EPOLLET | EPOLLIN;
        epoll_ctl(efd, EPOLL_CTL_ADD, srvFd, &ev);


        fd_ssl_map* fsm_head = (fd_ssl_map*)malloc(sizeof(fd_ssl_map));
        fsm_head->next = NULL;

        
        printf("server is listening...\n");
        static const char *https_response = 
            "HTTP/1.1 200 OK\r\nServer: httpd\r\nContent-Length: %d\r\nConnection: keep-alive\r\n\r\n";

        while (true) {
            // printf("epoll wait...\n");
            int nev = epoll_wait(efd, events, sizeof(events) / sizeof(struct epoll_event), -1);
            if (nev < 0) {
                printf("epoll_wait error. [%d,%s]", errno, strerror(errno));
                break;
            }

            for (size_t i = 0; i < nev; ++i) {
                // auto &event = events[i];
                struct epoll_event event = events[i];

                if (event.data.fd == srvFd) {  // accept
                    struct sockaddr_in addr;
                    socklen_t len = sizeof(addr);
                    int cfd = accept(srvFd, (struct sockaddr *)&addr, &len);
                    if (cfd > 0) {
                        printf("accept client %d [%s:%d]\n", cfd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                        SSL *ssl = SSL_new(ctx);
                        bool isSSLAccept = true;
                        if (ssl == NULL) {
                            printf("SSL_new error.\n");
                            continue;
                        }
                        int flags = fcntl(cfd, F_GETFL, 0);
                        fcntl(cfd, F_SETFL, flags | O_NONBLOCK);

                        SSL_set_fd(ssl, cfd);
                        int code;
                        int retryTimes = 0;

                        while ((code = SSL_accept(ssl)) <= 0 && retryTimes++ < 100) {
                            if (SSL_get_error(ssl, code) != SSL_ERROR_WANT_READ) {
                                printf("ssl accept error. %d\n", SSL_get_error(ssl, code));
                                break;
                            }
                            usleep(1);
                            printf("-------|");
                        }

                        printf("code %d, retry times %d\n", code, retryTimes);
                        if (code != 1) {
                            isSSLAccept = false;
                            close(cfd);
                            SSL_free(ssl);
                            continue;
                        }
                        ev.data.fd = cfd;
                        ev.events = EPOLLET | EPOLLIN;
                        epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &ev);
                        
                        fd_ssl_map* per = (fd_ssl_map*)malloc(sizeof(fd_ssl_map));
                        per->fd = cfd; per->ssl = ssl; per->next = NULL;
                        if(fsm_head->next == NULL)
                            fsm_head->next = per;
                        else{
                            per->next = fsm_head->next;
                            fsm_head->next = per;
                        }

                    } else {
                        perror("accept error");
                    }
                    continue;
                }

                fd_ssl_map* it = fsm_head;
                fd_ssl_map* pre;

                while(it->next != NULL){
                    if(it->fd == event.data.fd)
                        break;
                    pre = it;
                    it = it->next;
                }

                if (event.events & (EPOLLRDHUP | EPOLLHUP)) {
                    printf("client %d quit!\n", event.data.fd);
                    close(event.data.fd);
                    SSL_shutdown(it->ssl);
                    SSL_free(it->ssl);
                    
                    if(it->next == NULL)
                        pre->next = NULL;
                    pre->next = it->next;


                    epoll_ctl(efd, EPOLL_CTL_DEL, event.data.fd, NULL);
                    continue;
                }

                if (event.events & EPOLLIN) {
                    char buf[100] = {0};
                    int readSize = SSL_read(it->ssl, buf, sizeof(buf));
                    if (readSize <= 0) {
                        printf("SSL_read error. %d\n", SSL_get_error(it->ssl, readSize));
                        continue;
                    }
                    printf("read: %d\n%s\n", readSize, buf);

                    char sendBuf[1024] = {0};
                    int fmtSize = sprintf(sendBuf, https_response, readSize);

                    printf("*********************\n%s*********************\n", sendBuf);
                    int writeSize = SSL_write(it->ssl, sendBuf, strlen(sendBuf));    // 发送响应头
                    printf("format size %d, write size %d\n", fmtSize, writeSize);
                    if (writeSize <= 0) {
                        printf("SSL_write error. %d\n", SSL_get_error(it->ssl, writeSize));
                    }
                    writeSize = SSL_write(it->ssl, buf, readSize);   // 发送响应主体
                    if (writeSize <= 0) {
                        printf("SSL_write error. %d\n", SSL_get_error(it->ssl, writeSize));
                    }
                    printf("format size %d, write size %d\n", fmtSize, writeSize);
                }
            }
        }

        fd_ssl_map* it = fsm_head;
        while(it->next != NULL){
            close(it->fd);
            SSL_free(it->ssl);
        }
        

        SSL_CTX_free(ctx);
        close(srvFd);
        close(efd);
        return 0;
    }

#endif
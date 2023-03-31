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
#include <dmfserver/server.h>

#ifdef __WIN32__  // WIN32


void SSLservermake(ContFun cf[], char* keys[]){
	unsigned int myport, lisnum;

	SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (SSL_CTX_use_certificate_file(ctx, g_server_conf_all._conf_server.cert_public , SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, g_server_conf_all._conf_server.cert_private , SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }

	SOCKET sListen, sAccept;
    struct sockaddr_in ser, cli;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
    char buf[MAXBUF + 1]; 

    /* 开启一个 socket 监听 */
	sListen = socket(AF_INET, SOCK_STREAM, 0);

    ser.sin_family = AF_INET; 
    ser.sin_port = htons(443); 
    ser.sin_addr.s_addr = htonl(INADDR_ANY); 

    bind(sListen, (LPSOCKADDR)&ser, sizeof(ser) );
    listen(sListen,5);
	
	int iLen = sizeof(cli);
    while (1) {
        SSL *ssl;

        sAccept = accept(sListen, (struct sockaddr *)&cli, &iLen);

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sAccept);
        
        int ret = SSL_accept(ssl);
       
        printf("error code: %d %d\n", SSL_get_error(ssl, ret), ret);

        memset(buf, 0, MAXBUF + 1);
        int len = SSL_read(ssl, buf, MAXBUF);
        // printf("%s\n", buf);
        

        char res[128] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type:text/html\r\n"
                        "Server:Dmfserver\r\n"
                        "\r\n OK OK";
						
        len = SSL_write(ssl, res, strlen(res));
        

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sAccept);
    }

    close(sListen);
    SSL_CTX_free(ctx);
}


void Handler(int acceptFd, ContFun cf[], char* keys[]) {
	
	char res_str[RECEIVE_MAX_BYTES] = {'\0'};

	Request req1;
	int receive_bytes;
	receive_bytes = recv( acceptFd, res_str, sizeof(res_str), 0 );

	ParseHttp(&req1, res_str);
	
	char time [30] = {'\0'};
	serverTime(time);
	printf("[%s][Server: Info] %s\n",time , req1.path);
	
	
	Rou_init( cf, keys, acceptFd, &req1);
	//通过请求的 path 掉用了对应的处理函数
	
	freeReq(&req1);

}


void SimpleServerMake(ContFun cf[], char* keys[]) {
	
	int serverPort;
	// if configure not define port then use SERVER_PORT
	if(g_server_conf_all._conf_server.port == 0)
	serverPort = SERVER_PORT;
	else serverPort = g_server_conf_all._conf_server.port;

    WSADATA wsaData;
    SOCKET sListen, sAccept;

    struct sockaddr_in ser, cli;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    sListen = socket(AF_INET, SOCK_STREAM, 0);

    ser.sin_family = AF_INET; 
    ser.sin_port = htons(serverPort); 
    ser.sin_addr.s_addr = htonl(INADDR_ANY); 

    bind(sListen, (LPSOCKADDR)&ser, sizeof(ser) );
    listen(sListen,5);
    
	int iLen = sizeof(cli);
	

	while(1){
		sAccept = accept(sListen, (struct sockaddr *)&cli, &iLen);
		
		Handler( sAccept, cf, keys);
	}
	
    closesocket(sListen); //关闭 socket
    WSACleanup(); 
}



#elif __linux__  // linux 由于socket库不同 要重写三个函数


void SSLservermake(ContFun cf[], char* keys[]){

	unsigned int myport, lisnum;

	SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (SSL_CTX_use_certificate_file(ctx, g_server_conf_all._conf_server.cert_public , SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, g_server_conf_all._conf_server.cert_private , SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }

	int sListen, sAccept;
    struct sockaddr_in ser, cli;
	

    char buf[MAXBUF + 1]; 

	sListen = socket(AF_INET, SOCK_STREAM, 0);

    ser.sin_family = AF_INET; 
    ser.sin_port = htons(443); 
    ser.sin_addr.s_addr = htonl(INADDR_ANY); 

    bind(sListen, (struct sockaddr*)&ser, sizeof(ser) );
    listen(sListen,5);
	
	int iLen = sizeof(cli);
    while (1) {
        SSL *ssl;

        sAccept = accept(sListen, (struct sockaddr *)&cli, &iLen);

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sAccept);
        
        int ret = SSL_accept(ssl);
       
        printf("error code: %d %d\n", SSL_get_error(ssl, ret), ret);

        memset(buf, 0, MAXBUF + 1);
        int len = SSL_read(ssl, buf, MAXBUF);
        // printf("%s\n", buf);
        

        char res[128] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type:text/html\r\n"
                        "Server:Dmfserver\r\n"
                        "\r\n OK OK";
						
        len = SSL_write(ssl, res, strlen(res));
        

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sAccept);
    }

    close(sListen);
    SSL_CTX_free(ctx);

}


void Handler(int acceptFd, ContFun cf[], char* keys[]) {
	
	char res_str[RECEIVE_MAX_BYTES] = {'\0'};

	Request req1;
	int receive_bytes;
	receive_bytes = recv( acceptFd, res_str, sizeof(res_str), 0 );

	ParseHttp(&req1, res_str);
	
	char time [30] = {'\0'};
	serverTime(time);
	printf("[%s][Server: Info] %s\n",time , req1.path);
	
	
	Rou_init( cf, keys, acceptFd, &req1);
	//通过请求的 path 掉用了对应的处理函数
	
	freeReq(&req1);
}


void SimpleServerMake(ContFun cf[], char* keys[]) {
	
	int serverPort;
	// if configure not define port then use SERVER_PORT
	if(g_server_conf_all._conf_server.port == 0)
	serverPort = SERVER_PORT;
	else serverPort = g_server_conf_all._conf_server.port;


    int sListen, sAccept;
    struct sockaddr_in ser, cli;


    sListen = socket(AF_INET, SOCK_STREAM, 0);

    ser.sin_family = AF_INET; 
    ser.sin_port = htons(serverPort);
	ser.sin_addr.s_addr = htonl(INADDR_ANY);
     
    bind(sListen, (struct sockaddr*)&ser, sizeof(ser) );
    listen(sListen,5);
    
	int iLen = sizeof(cli);
	
	printf("Simple Server is running! \n");
	while(1){
		sAccept = accept(sListen, (struct sockaddr *)&cli, &iLen);
		Handler( sAccept, cf, keys);
	}
	
    close(sListen); //关闭 socket

}


int createSocket() 
{
	int i_listenfd;
	struct sockaddr_in st_sersock;
	i_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&st_sersock, 0, sizeof(st_sersock));
	st_sersock.sin_family = AF_INET;
//	st_sersock.sin_addr.s_addr = htonl(INADDR_ANY);
	st_sersock.sin_port = htons( 8080 );
	int out = 2;
	setsockopt(i_listenfd, SOL_SOCKET, SO_REUSEADDR, &out, sizeof(out));
	bind(i_listenfd,(struct sockaddr*)&st_sersock, sizeof(st_sersock));
	listen(i_listenfd, 20);
	return i_listenfd;
}


typedef struct thread_arg {
	ContFunMap cmp;
	long fd;
} thread_arg;


void* threadingServerMake(void* p)
{	
	thread_arg* arg = (thread_arg*)p;
	long i_listenfd = arg->fd;

	struct epoll_event ev, events[1024];
	int epfd, nCounts;
	int i_connfd;
	epfd = epoll_create(1024);

	ev.events = EPOLLIN | EPOLLEXCLUSIVE;
	ev.data.fd = i_listenfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, i_listenfd, &ev);

	Request req1;
 	char time [30] = {'\0'};

	while(1)
	{
		nCounts = epoll_wait(epfd, events, 1024, -1);
		for(int i = 0; i < nCounts; i++)
		{
			int tmp_epoll_recv_fd = events[i].data.fd;
			if(tmp_epoll_recv_fd == i_listenfd)	{
				i_connfd = accept(i_listenfd, (struct sockaddr*)NULL, NULL);	
				ev.events = EPOLLIN;
				ev.data.fd = i_connfd;
				epoll_ctl( epfd, EPOLL_CTL_ADD, i_connfd, &ev );
			} else {
				char res_str[RECEIVE_MAX_BYTES] = {'\0'};
				int receive_bytes;
				receive_bytes = recv( tmp_epoll_recv_fd, res_str, sizeof(res_str), 0 );
				ParseHttp(&req1, res_str);
				serverTime(time);
				printf("[%s][Server: Info] %s %d id: %d\n",time , req1.path, (int)strlen(res_str), getppid());
				memset(time, 0, 30);
				
				Rou_iocp_handle(arg->cmp, tmp_epoll_recv_fd, &req1);
				
				freeReq(&req1);
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



int threadingServerRunning() {
	

	long fd = createSocket();
	thread_arg* ta = malloc(sizeof(thread_arg));
	ta->cmp = g_cmp;
	ta->fd = fd;
	
	if(fd == 0) {return 1;}

	for (int i = 0; i < 4; ++i) {
		pthread_t roundCheck;
		pthread_create(&roundCheck, NULL, threadingServerMake, (void*)ta);
		pthread_join(roundCheck, NULL);
	}
	
	
	return 0;
}

#endif // linux






#ifdef __WIN32__  // WIN32 windows 启动IOCP服务器

SOCKET BindServerOverlapped(int nPort){

	SOCKET sServer = WSASocket(AF_INET,SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);


	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(nPort);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sServer, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		OutErr("bind Failed!");
		return 1;
	}
	if(listen(sServer, 200) != 0)
	{
		OutErr("listen Failed!");
		return 1;
	}
	return sServer;
}


DWORD WINAPI ProcessIO(LPVOID lpParam){
	
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    LPPER_HANDLE_DATA PerHandleData;
    LPPER_IO_OPERATION_DATA PerIoData;
 	Request req1;
 	char time [30] = {'\0'};

	while(1){

		if(0 == GetQueuedCompletionStatus(CompletionPort,&BytesTransferred, (LPDWORD)&PerHandleData,(LPOVERLAPPED*)&PerIoData, INFINITE)){
			if( (GetLastError() ==WAIT_TIMEOUT)) { //|| (GetLastError() == ERROR_NETNAME_DELETED ) ){
				printf("closingsocket %d\n", PerHandleData->Socket); 
				closesocket(PerHandleData->Socket);

				free( PerIoData);
				free( PerHandleData);
				continue;
			}else{
				OutErr("GetQueuedCompletionStatus failed!");
			}
		return 0;
		}

		// 说明客户端已经退出
		if(BytesTransferred == 0)
		{
			printf("closingsocket %d\n", PerHandleData->Socket);
			closesocket(PerHandleData->Socket);
			free( PerIoData);
			free( PerHandleData);
			continue;
		}
		
		ParseHttp(&req1, PerIoData->Buffer);
		serverTime(time);
		printf("[%s][Server: Info] %s %d id: %d\n",time , req1.path, strlen(PerIoData->Buffer), GetCurrentThreadId ());
		memset(time, 0, 30);
		
		Rou_iocp_handle(PerIoData->cmp, PerHandleData->Socket, &req1);
		
		freeReq(&req1);
		
		free( PerIoData );
		free( PerHandleData );

		/*
		// 继续向 socket 投递WSARecv操作
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		ZeroMemory(PerIoData,sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf =PerIoData->Buffer;
		PerIoData->DataBuf.len = DATA_BUFSIZE;
		WSARecv(PerHandleData->Socket,&PerIoData->DataBuf, 1, &dwRecv, &Flags,&PerIoData->Overlapped, NULL);
		*/
	}
 
	return 0;
}


int iocpServerMake(ContFunMap cmp) {

	int serverPort;
	WSADATA wsd;
	if( WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		OutErr("WSAStartup()");
	}
	
	HANDLE CompletionPort =CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	for(int i = 0; i <SystemInfo.dwNumberOfProcessors * 2; i++){
	// for(int i=0; i<30; i++){
		
		   HANDLE hProcessIO = CreateThread(NULL, 0, ProcessIO, CompletionPort, 0, NULL);
		   if(hProcessIO)
		   {
				  CloseHandle(hProcessIO);
		   }
	}

	
	// if configure not define port then use SERVER_PORT
	if(g_server_conf_all._conf_server.port == 0)
	serverPort = SERVER_PORT;
	else serverPort = g_server_conf_all._conf_server.port;
	// Listening socket
	SOCKET sListen = BindServerOverlapped(serverPort);

	SOCKET sClient;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_OPERATION_DATA PerIoData;


	
	while(1){
		
		// wait for client
		sClient = WSAAccept(sListen, NULL, NULL, NULL, 0);
		//sClient = accept(sListen, 0, 0);

		PerHandleData =  (PER_HANDLE_DATA*)malloc(sizeof(PER_HANDLE_DATA));
		PerHandleData->Socket = sClient;
		// 建立一个Overlapped，并使用这个Overlapped结构对socket投递操作
		PerIoData =  (PER_IO_OPERATION_DATA*)malloc(sizeof(PER_IO_OPERATION_DATA));

		// 将接入的客户端和完成端口联系起来
		CreateIoCompletionPort((HANDLE)sClient, CompletionPort,(DWORD)PerHandleData, 0);

		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = DATA_BUFSIZE;
		PerIoData->cmp = cmp;

		// 投递一个WSARecv操作
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		WSARecv(sClient, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}

	DWORD dwByteTrans;
	//将一个已经完成的IO通知添加到IO完成端口的队列中.
	//提供了与线程池中的所有线程通信的方式.
	PostQueuedCompletionStatus(CompletionPort,dwByteTrans, 0, 0);  //IO操作完成时接收的字节数.

	closesocket(sListen);
	
	return 0;
}

#endif   // WIN32
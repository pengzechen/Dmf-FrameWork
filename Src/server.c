
#include "server.h"

clock_t start, stop;

void SSLservermake(ContFun cf[], char* keys[]){
	unsigned int myport, lisnum;

	SSL_CTX *ctx;

    /* SSL 库初始化 */
    SSL_library_init();
    /* 载入所有 SSL 算法 */
    OpenSSL_add_all_algorithms();
    /* 载入所有 SSL 错误消息 */
    SSL_load_error_strings();
    /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
    ctx = SSL_CTX_new(SSLv23_server_method());
    /* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }

	
	
    /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
    if (SSL_CTX_use_certificate_file(ctx, "localhost.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(ctx, "localhost-key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 检查用户私钥是否正确 */
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
	start = clock();    /*  开始计时  */
	
	
	char res_str[RECEIVE_MAX_BYTES] = {'\0'};

	Request req1;
	int receive_bytes;
	receive_bytes = recv( acceptFd, res_str, sizeof(res_str), 0 );

	ParseHttp(&req1, res_str);
	
	char time [30] = {'\0'};
	serverTime(time);
	printf("[%s][Server: Info] %s\n",time , req1.path);
	
	
	Rou_init( cf, keys, 3, acceptFd, &req1);
	//通过请求的 path 掉用了对应的处理函数
	
	freeReq(&req1);
	stop = clock();     /*  停止计时  */
		
	// printf("[server]Handel Time: %Lf\n", ((double)(stop - start)) / CLK_TCK );
}


void SimpleServerMake(ContFun cf[], char* keys[]) {
	
	int serverPort = SERVER_PORT;
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
		/*
		if(0 == GetQueuedCompletionStatus(CompletionPort,&BytesTransferred, (LPDWORD)&PerHandleData,(LPOVERLAPPED*)&PerIoData, INFINITE))
		{
		if( (GetLastError() ==WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED) )
		{
			cout << "closingsocket" << PerHandleData->Socket << endl; 
			closesocket(PerHandleData->Socket);

			free( PerIoData);
			free( PerHandleData);
			continue;
		}
		else
		{
			OutErr("GetQueuedCompletionStatus failed!");
		}
		return 0;
		}

		// 说明客户端已经退出
		if(BytesTransferred == 0)
		{
			cout << "closing socket" <<PerHandleData->Socket << endl;
			closesocket(PerHandleData->Socket);
			free( PerIoData);
			free( PerHandleData);
			continue;
		}
		*/
		GetQueuedCompletionStatus(CompletionPort,&BytesTransferred, (LPDWORD)&PerHandleData,(LPOVERLAPPED*)&PerIoData, INFINITE);

		
		ParseHttp(&req1, PerIoData->Buffer);
		serverTime(time);
		printf("[%s][Server: Info] %s %d id: %d\n",time , req1.path, strlen(PerIoData->Buffer), GetCurrentThreadId ());
		memset(time, 0, 30);
		
		Rou_iocp_init(PerIoData->cmp, PerHandleData->Socket, &req1);
		
		freeReq(&req1);
		// send(PerHandleData->Socket, "HTTP/1.1 200 \r\n\r\n hello", 23,0);
		// closesocket(PerHandleData->Socket);
		
		free( PerIoData);
		free( PerHandleData);

		continue;

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


int iocpServerMake(ContFunMap cmp){

	SessionInit(session_all_dec);
	mysql_pool_init();
	elr_mpl_init();
	ConfInit();
	
	
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

	//创建侦听SOCKET
	SOCKET sListen = BindServerOverlapped(SERVER_PORT);

	SOCKET sClient;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_OPERATION_DATA PerIoData;


	
	while(1){
		
		// 等待客户端接入
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
		WSARecv(sClient, &PerIoData->DataBuf, 1, &dwRecv, &Flags,&PerIoData->Overlapped, NULL);
	}

	DWORD dwByteTrans;
	//将一个已经完成的IO通知添加到IO完成端口的队列中.
	//提供了与线程池中的所有线程通信的方式.
	PostQueuedCompletionStatus(CompletionPort,dwByteTrans, 0, 0);  //IO操作完成时接收的字节数.

	closesocket(sListen);
	
	return 0;
}

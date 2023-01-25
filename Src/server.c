
#include "server.h"

clock_t start, stop;

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


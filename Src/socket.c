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


   
#include <dmfserver/socket.h>

int create_socket()
{
    
	int serverPort;
	// if configure not define port, we use default SERVER_PORT
	if(g_server_conf_all._conf_server.port == 0)
	serverPort = SERVER_PORT;
	else serverPort = g_server_conf_all._conf_server.port;

    int sListen;
    struct sockaddr_in ser;
    sListen = socket(AF_INET, SOCK_STREAM, 0);
    ser.sin_family = AF_INET; 
    ser.sin_port = htons(80); 
    ser.sin_addr.s_addr = htonl(INADDR_ANY); 
    if( bind(sListen, (struct sockaddr*)&ser, sizeof(ser) ) < 0) 
    {
		OutErr("bind Failed!");
		return 1;
	}
    if( listen(sListen,5) != 0) 
    {
		OutErr("listen Failed!");
		return 1;
	}

    return sListen;
}


#ifdef __linux__

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
    int code = bind(i_listenfd, (struct sockaddr *)&st_sersock, sizeof(st_sersock));
    assert(code == 0 && "bind error");

    code = listen(i_listenfd, 1024);
    assert(code == 0 && "listen error");
    return i_listenfd;
}


int create_socket_reuse()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd > 0 && "socket error");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    // addr.sin_addr.s_addr = inet_addr(LOCAL_IP);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    addr.sin_port = htons(80);

    int flag = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    int code = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    assert(code == 0 && "bind error");

    code = listen(fd, 1024);
    assert(code == 0 && "listen error");

    struct timeval tv = {0, 500 * 1000};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    return fd;
}


#endif // linux


#ifdef __WIN32__

SOCKET iocp_bind_socket(int nPort)
{

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

#endif // __win32__ 
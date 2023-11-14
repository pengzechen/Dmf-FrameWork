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

#include <dmfserver/mdb/mdb.h>


#ifdef __WIN32__
#include <winerror.h>
#include <Winsock2.h>
#include <windows.h>
#include <stdio.h>


#define PORT 5555
#define DATA_BUFSIZE 2048
#define OutErr(a) printf("%s %s %d %d", a, WSAGetLastError(), __FILE__ , __LINE__ );


typedef struct{
   OVERLAPPED Overlapped;
   WSABUF DataBuf;
   CHAR Buffer[DATA_BUFSIZE];
   
}per_io_data_t,* per_io_data_tp;
 
 
typedef struct{
   SOCKET Socket;
   int status;
   
}per_handle_data_t,* per_handle_data_tp;

static HANDLE mdb_mapping;
static HANDLE mdb_mutex;



void mdb_init()
{
	mdb_mapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		MAX_ENTRIES * sizeof(entry_t),
		SHARED_STR
	);
	if (mdb_mapping == NULL) {
		printf("Failed to create file mapping object. Error code: %d\n", GetLastError());
		return ;
	}

	mdb_mutex = CreateMutex(NULL, FALSE, SHARED_MUTEX);
	if (mdb_mutex == NULL) {
		printf("Failed to create mutex. Error code: %d\n", GetLastError());
		// UnmapViewOfFile(shared_data);
		CloseHandle(mdb_mapping);
		return ;
	}
}

extern unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131;   // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*str){
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

char* mdb_find(char* key)
{

    entry_t *shared_data;
    int flag = 0;
    char* res;
    unsigned int ihash;


    shared_data = (entry_t *)MapViewOfFile(
        mdb_mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        MAX_ENTRIES * sizeof(entry_t)
    );

    if (shared_data == NULL) {
        printf("Failed to map view of file. Error code: %d\n", GetLastError());
        //CloseHandle(mdb_mapping);
        return "";
    }

    ihash = BKDRHash(key);
    if(ihash >= MAX_ENTRIES)
        ihash = ihash % MAX_ENTRIES;

    WaitForSingleObject(mdb_mutex, INFINITE);

    if (strcmp(shared_data[ihash].key, key) == 0) {
        flag = 1;
    }

    ReleaseMutex(mdb_mutex);

    if(flag) {
        res =  (char*)malloc(sizeof(char)*MAX_VALUE_LEN) ;
        strcpy(res, shared_data[ihash].value);
        UnmapViewOfFile(shared_data);
        return res;
    }
    
    UnmapViewOfFile(shared_data);
    return "";
    
}

void mdb_insert(char* key, char* value) 
{
    entry_t *shared_data;
    unsigned int ihash;

    shared_data = (entry_t *)MapViewOfFile(
        mdb_mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        MAX_ENTRIES * sizeof(entry_t)
    );

    if (shared_data == NULL) {
        printf("Failed to map view of file. Error code: %d\n", GetLastError());
        //CloseHandle(mdb_mapping);
        return ;
    }

    ihash = BKDRHash(key);
    if(ihash >= MAX_ENTRIES)
        ihash = ihash % MAX_ENTRIES;
    
    WaitForSingleObject(mdb_mutex, INFINITE);
    strncpy(shared_data[ihash].key, key, MAX_KEY_LEN);
    strncpy(shared_data[ihash].value, value, MAX_VALUE_LEN);
    ReleaseMutex(mdb_mutex);

    UnmapViewOfFile(shared_data);
}


SOCKET BindServerOverlapped(int nPort)	
{
	SOCKET sServer = WSASocket(AF_INET,SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(nPort);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sServer, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0){
		OutErr("bind Failed!");
		return 1;
	}
	if(listen(sServer, 200) != 0){
		OutErr("listen Failed!");
		return 1;
	}
	return sServer;
}
 
void next(per_handle_data_t* PerHandleData, per_io_data_t* PerIoData)
{
 
	DWORD Flags = 0;
	DWORD dwRecv = 0;
	ZeroMemory(PerIoData,sizeof(per_io_data_t));
	PerIoData->DataBuf.buf = PerIoData->Buffer;
	PerIoData->DataBuf.len = DATA_BUFSIZE;
	WSARecv(PerHandleData->Socket,&PerIoData->DataBuf, 1, &dwRecv, &Flags,&PerIoData->Overlapped, NULL);
	PerHandleData->status = 1;
}

DWORD WINAPI ProcessIO(LPVOID lpParam)	
{
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    per_handle_data_tp PerHandleData;
    per_io_data_tp PerIoData;
 
	while(1){
		if(0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&PerHandleData, 
										(LPOVERLAPPED*)&PerIoData, 
										INFINITE))
		{
			if( (GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED) ){
				printf( "closingsocket %d\n", PerHandleData->Socket ); 
				closesocket(PerHandleData->Socket);
				free( PerIoData);
				free( PerHandleData);
				continue;
			}
			else{
				OutErr("GetQueuedCompletionStatus failed!");
			}
			return 0;
		}
		if(BytesTransferred == 0){
			printf( "closing socket %d \n" ,PerHandleData->Socket);
			closesocket(PerHandleData->Socket);
			free( PerIoData);
			free( PerHandleData);
			continue;
		}
		
		if(PerHandleData->status == 0){
			if(strcmp(PerIoData->Buffer, "123") == 0) {
				printf( "%d authorized\n", PerHandleData->Socket);
				send(PerHandleData->Socket, "authorized", 10, 0);
				next(PerHandleData, PerIoData);
			}else{
				printf( "reject socket %d\n" ,PerHandleData->Socket);
				closesocket(PerHandleData->Socket);
				free( PerIoData);
				free( PerHandleData);
				continue;
			}
		}else{
			// printf( "%d Send\n", PerHandleData->Socket);
			entry_t* et = (entry_t*)&PerIoData->Buffer;

			if(et->op == 1) {
				printf("KEY:%s, VALUE:%s inserted\n", et->key, et->value);
				mdb_insert(et->key, et->value);
				send(PerHandleData->Socket, "insert ok", 9, 0);
			}else if (et->op == 2) {
				printf("FIND:%s \n", et->key);
				char* data = mdb_find(et->key);
				send(PerHandleData->Socket, data, strlen(data), 0);
				free(data);
			}
			next(PerHandleData, PerIoData);
		}
	}
 	return 0;
}

#endif // WIN32

int main(){
#ifdef __WIN32__
	mdb_init();
	
	WSADATA wsd;
	if( WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
		OutErr("WSAStartup()");

	HANDLE CompletionPort =CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	for(int i = 0; i <SystemInfo.dwNumberOfProcessors * 2; i++){
		HANDLE hProcessIO = CreateThread(NULL, 0, ProcessIO, CompletionPort, 0, NULL);
		if(hProcessIO)
			CloseHandle(hProcessIO);
	}

	SOCKET sListen = BindServerOverlapped(PORT);

	SOCKET sClient;
	per_handle_data_tp PerHandleData;
	per_io_data_tp PerIoData;
	while(1)
	{
		   sClient = accept(sListen, 0, 0);
		   printf("Socket %d connected \n", sClient);

		   PerHandleData =  (per_handle_data_t*)malloc(sizeof(per_handle_data_t));
		   PerHandleData->Socket = sClient;
		   PerHandleData->status = 0;
		   
		   PerIoData =  (per_io_data_t*)malloc(sizeof(per_io_data_t));
		   CreateIoCompletionPort((HANDLE)sClient, CompletionPort,(DWORD)PerHandleData, 0);

		   ZeroMemory(PerIoData, sizeof(per_io_data_t));
		   PerIoData->DataBuf.buf = PerIoData->Buffer;
		   PerIoData->DataBuf.len = DATA_BUFSIZE;

		   DWORD Flags = 0;
		   DWORD dwRecv = 0;
		   WSARecv( sClient, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}

	DWORD dwByteTrans;

	PostQueuedCompletionStatus(CompletionPort, dwByteTrans, 0, 0);  //IO操作完成时接收的字节数.

	closesocket(sListen);
#endif // WIN32
	return 0;
}

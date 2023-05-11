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

#include <dmfserver/mdb/mdb_operate.h>


#ifdef __WIN32__

void c_close(SOCKET hSocket)
{
	closesocket(hSocket);
	WSACleanup();
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int sock_main()
{
	
	SOCKET hSocket;
	struct sockaddr_in servAddr;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) ErrorHandling("socket() error");
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(5555);

	if(connect(hSocket, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling("connect() error!");
	
	char message[512] = {0};
	int recv_len;
	int send_len;

printf("-------------login\n");
	send_len = send(hSocket, "123", 3, 0);
	recv_len = recv(hSocket, message, sizeof(message) - 1, 0);
	printf("| Message from server: %s \n", message);
	memset(message, 0, 30);
printf("-------------login----------------\n\n");


printf("-------------insert\n");
entry_t et;
et.op = 1;
strcpy(et.key, "name");
strcpy(et.value, "pengzechen");
	send_len = send(hSocket, (char*)&et, sizeof(entry_t), 0);
	recv_len = recv(hSocket, message, sizeof(message) - 1, 0);
	printf("| Message from server: %s \n", message);
	memset(message, 0, 30);
printf("-------------insert----------------\n");


printf("-------------find\n");
memset(&et, 0, sizeof(entry_t));
et.op = 2;
strcpy(et.key, "name");
	send_len = send(hSocket, (char*)&et, sizeof(entry_t), 0);
	recv_len = recv(hSocket, message, sizeof(message) - 1, 0);
	printf("| Message from server: %s \n", message);
	memset(message, 0, 30);
printf("-------------find----------------\n");

	return 0;
}

void shm_main() 
{
    while (1) {
        printf("Enter 0 to retrieve value, 1 to insert value (or q to quit): ");
        char input[MAX_VALUE_LEN];
        if (fgets(input, MAX_VALUE_LEN, stdin) == NULL) {
            break;
        }

        if (strcmp(input, "q\n") == 0) {
            break;
        }

        int op = atoi(input);
        if (op == 0) {
            // Query operation
            char key[MAX_KEY_LEN];

            printf("Enter key to retrieve value (or q to quit): ");
            if (fgets(key, MAX_KEY_LEN, stdin) == NULL) {
                break;
            }

            if (strcmp(key, "q\n") == 0) {
                break;
            }

            printf("%s\n", mdb_find(key));
        } else if (op == 1) {
            // Insert operation
            char key[MAX_KEY_LEN];
            char value[MAX_VALUE_LEN];

            printf("Enter key: ");
            if (fgets(key, MAX_KEY_LEN, stdin) == NULL) {
                break;
            }

            printf("Enter value: ");
            if (fgets(value, MAX_VALUE_LEN, stdin) == NULL) {
                break;
            }

            mdb_insert(key, value);
			
		} else {
			printf("Invalid input\n");
		}
	}
}

#endif // __WIN32__

int main(int argc, char* args[]) {
    
    #ifdef __WIN32__

    if(argc != 2){
        printf("mdbclient -s\nmdbclient -m\n");
        getchar();
        exit(1);
    }
        
    if(strcmp(args[1], "-s") ==0 )
    sock_main();
    if(strcmp(args[1], "-m") ==0 )
    shm_main();
    
    #endif // __WIN32__

	return 0;
}

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
#include "views.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpRserved){
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
			// printf("dll_process_attach\n");
			break;
		case DLL_PROCESS_DETACH:
			// printf("dll_process_detach\n");
			break;
		case DLL_THREAD_ATTACH:
			// printf("dll_thread_attach\n");
			break;
		case DLL_THREAD_DETACH:
			// printf("dll_thread_detach\n");
			break;
		default:
			break;
	}

	return TRUE;
}

void set(int a){
	testlink = a;
}

int get(){
	return testlink;
}

void viewtest(int a, const Request* req)
{
	printf("--in--%s\n", g_server_conf_all._conf_model.database);
	exeSql("select * from test;");
	Res_row(a, "lib test ok\n ");
}

void dll_read_shm(){
	HANDLE hMapFile;
	LPCTSTR pBuf;
	// "Global\\MyFileMappingObject"

	TCHAR szName[]=TEXT("MyFileMappingObject");

	hMapFile = OpenFileMapping(
		FILE_MAP_READ,
		FALSE,
		szName);

	if (hMapFile == NULL){
		MessageBox(NULL, "Failed to open file mapping.\n", "Error", MB_ICONERROR);
		DWORD error = GetLastError();
		printf("%d\n", error);
		return;
	}
	pBuf = (LPCTSTR)MapViewOfFile(
		hMapFile, // 映射文件句柄
		FILE_MAP_READ, // 可读可写
		0,
		0,
		1024*1024);
	if(pBuf == NULL){
		MessageBox(NULL, "Failed to view of file.", "Error", MB_ICONERROR);
		CloseHandle(hMapFile);
		return;
	}

	char str[1024*1024];

	ZeroMemory(str, 1024*1024);
	strncpy(str, (char*)pBuf, 1024*1024-1);

	MessageBox(NULL, str, "Shared Memory", MB_OK);

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}

void dll_write_shm(char* str){
	TCHAR szName[]=TEXT("MyFileMappingObject");
	HANDLE hMapFile;
	LPCTSTR pBuf;
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE, //  使用物理文件来映射内存
		NULL, // 不需要安全描述符
		PAGE_READWRITE,  //可读可写
		0,
		1024*1024,
		szName);
	if (hMapFile == NULL){
		DWORD error = GetLastError();
		printf("%d\n", error);
		MessageBox(NULL, "Failed to create file mapping.\n", "Error", MB_ICONERROR);
		return;
	}
	pBuf = (LPCTSTR)MapViewOfFile(
		hMapFile, // 映射文件句柄
		FILE_MAP_ALL_ACCESS, // 可读可写
		0,
		0,
		1024*1024);
	if(pBuf == NULL){
		MessageBox(NULL, "Failed to view of file.", "Error", MB_ICONERROR);
		CloseHandle(hMapFile);
		return;
	}
	ZeroMemory((PVOID)pBuf, 1024*1024);
	strncpy((char*)pBuf, str, 1024*1024);

	printf("give string: %s\n", str);
	UnmapViewOfFile(pBuf);
	// CloseHandle(hMapFile);
}


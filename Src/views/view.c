#include <server.h>

#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)

// 多进程实现数据共享的方法
#pragma data_seg("flag_data")
int testlink = 0;
char string[1024*1024] = {0};
#pragma data_seg()
#pragma comment(linker,"/SECTION:flag_data,RWS")


/*
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpRserved){
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
			printf("dll_process_attach\n");
			break;
		case DLL_PROCESS_DETACH:
			printf("dll_process_detach\n");
			break;
		case DLL_THREAD_ATTACH:
			printf("dll_thread_attach\n");
			break;
		case DLL_THREAD_DETACH:
			printf("dll_thread_detach\n");
			break;
		default:
			break;
	}

	return TRUE;
}
*/

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

DLL_EXPORT void set(int a){
	testlink = a;
}
DLL_EXPORT int get(){
	return testlink;
}
DLL_EXPORT void viewtest(int a, const Request* req)
{
	// printf("--in--%s\n", g_server_conf_all._conf_model.database);
	Res_row(a, "lib test ok\n ");
}

DLL_EXPORT void read_shm(){
	HANDLE hMapFile;
	LPCTSTR pBuf;
	TCHAR szName[]=TEXT("Global/MyFileMappingObject");

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
		FILE_MAP_ALL_ACCESS, // 可读可写
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

DLL_EXPORT void dll_write_to_shm(char* str){
	TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
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
	CloseHandle(hMapFile);
}


#ifdef __cplusplus
}
#endif
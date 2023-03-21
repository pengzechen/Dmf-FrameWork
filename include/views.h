#ifndef __views__
#define __views__

#include <server.h>

#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)

// 多进程实现数据共享的方法
#pragma data_seg("flag_data")
int testlink = 0;
char string[1024*1024] = {0};
#pragma data_seg()
#pragma comment(linker,"/SECTION:flag_data,RWS")

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

	DLL_EXPORT void set(int a);

	DLL_EXPORT int get();

	DLL_EXPORT void viewtest(int a, const Request* req);

	DLL_EXPORT void dll_read_shm();

	DLL_EXPORT void dll_write_shm(char* str);

#ifdef __cplusplus
}
#endif

#endif
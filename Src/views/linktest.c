#include <windows.h>
#include <stdio.h>

typedef int(*Get)();
typedef void(*Set)(int);

int main() {
    HMODULE handle = LoadLibrary("./views/libviews.dll");
    Get link_get = (Get)GetProcAddress(handle, "get");
	Set link_set = (Set)GetProcAddress(handle, "set");

	printf("link test %d \n", link_get());

    while(1){Sleep(2000);}
    FreeLibrary(handle);
    return 0;
}
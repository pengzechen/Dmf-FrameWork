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
#include <windows.h>
#include <stdio.h>

typedef int(*Get)();
typedef void(*Set)(int);
typedef void(*Dll_read_shm)();
typedef void(*Dll_write_shm)(char*);

int main() {
    HMODULE handle = LoadLibrary("./views/libviews.dll");
    
    Get link_get = (Get)GetProcAddress(handle, "get");
	Set link_set = (Set)GetProcAddress(handle, "set");
    link_set(3);

    Dll_read_shm dll_read_shm = (Dll_read_shm)GetProcAddress(handle, "dll_read_shm");
    Dll_write_shm dll_write_shm = (Dll_write_shm)GetProcAddress(handle, "dll_write_shm");

	
    char data[] = "this is a shm test data";
    dll_write_shm(data);


    while(1){Sleep(2000);printf("[linktest: ]%d\n", link_get());}
    FreeLibrary(handle);
    return 0;
}
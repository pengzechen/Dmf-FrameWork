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
#include <dmfserver/mdb/share.h>

#ifdef __WIN32__

void share_test() {
    HMODULE handle = LoadLibrary("./libmdb.dll");
    Get link_get = (Get)GetProcAddress(handle, "get");
	Set link_set = (Set)GetProcAddress(handle, "set");
    link_set(3);
    Dll_read_shm dll_read_shm = (Dll_read_shm)GetProcAddress(handle, "dll_read_shm");
    Dll_write_shm dll_write_shm = (Dll_write_shm)GetProcAddress(handle, "dll_write_shm");
    char data[] = "this is a shm test data";
    dll_write_shm(data);
    int i = 0 ;
    while( i != 10){
        dll_read_shm();
        Sleep(2000);
        i++;
    }
    FreeLibrary(handle);
}

void share_test2()
{
    ShellExecute(NULL, "open", "D:\\C\\request\\bin\\mdb\\mdb1.exe", NULL, NULL, SW_SHOW);
    Sleep(1000);
    HMODULE handle = LoadLibrary("./mdb/libmdb.dll");
    Get link_get = (Get)GetProcAddress(handle, "get");
    Set link_set = (Set)GetProcAddress(handle, "set");
    Dll_read_shm dll_read_shm = (Dll_read_shm)GetProcAddress(handle, "dll_read_shm");
    dll_read_shm();
    Dll_write_shm dll_write_shm = (Dll_write_shm)GetProcAddress(handle, "dll_write_shm");
    dll_write_shm("rewrite shm data");
}
#endif //WIN32


int main() {
#ifdef __WIN32__
#endif // __win32__
    return 0;
}
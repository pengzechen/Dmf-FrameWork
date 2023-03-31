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
#ifndef __link__
#define __link__


#include <stdio.h>
typedef int(*Get)();
typedef void(*Set)(int);



#ifdef __WIN32__
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#include <windows.h>
typedef void(*Dll_read_shm)();
typedef void(*Dll_write_shm)(char*);
#endif  // WIN32


#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

#ifdef __WIN32__
	DLL_EXPORT void set(int a);

	DLL_EXPORT int get();

	DLL_EXPORT void dll_read_shm();

	DLL_EXPORT void dll_write_shm(char* str);
#elif __linux__			// WIN32
	void set(int a);

	int get();
#endif // linux


#ifdef __cplusplus
}    /* end of the 'extern "C"' block */
#endif

#endif // __link__
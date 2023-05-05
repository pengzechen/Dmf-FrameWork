
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


// 人民群众是社会物质的创造者
// 人民群众是社会精神的创造者
// 人民群众是社会变革的决定力量

#ifdef __WIN32__

    static HANDLE file_mapping;
    static HANDLE mutex;

    BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpRserved){
        switch(ul_reason_for_call){
            case DLL_PROCESS_ATTACH:
                // printf("dll_process_attach\n");
                mdb_operate_init();
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

    /*
        #define DLL_EXPORT __declspec(dllexport)
        typedef int(*Get)();
        HMODULE handle = LoadLibrary("./libshare.dll");
        Get link_get = (Get)GetProcAddress(handle, "get");
        Set link_set = (Set)GetProcAddress(handle, "set");
    */

    void mdb_operate_init() 
    {
        file_mapping = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            MAX_ENTRIES * sizeof(entry_t),
            SHARED_STR
        );
        if (file_mapping == NULL) {
            printf("Failed to create file mapping object. Error code: %d\n", GetLastError());
            return ;
        }

        mutex = CreateMutex(NULL, FALSE, SHARED_MUTEX);
        if (mutex == NULL) {
            printf("Failed to create mutex. Error code: %d\n", GetLastError());
            // UnmapViewOfFile(shared_data);
            CloseHandle(file_mapping);
            return ;
        }

        printf("[Mdb: Info]: mdb init successfully! \n");
        printf("\n");
    }

    char* mdb_find(char* key)
    {

        entry_t *shared_data;
        int flag = 0;
        char* res;
        unsigned int ihash;


        shared_data = (entry_t *)MapViewOfFile(
            file_mapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            MAX_ENTRIES * sizeof(entry_t)
        );

        if (shared_data == NULL) {
            printf("Failed to map view of file. Error code: %d\n", GetLastError());
            //CloseHandle(file_mapping);
            return "";
        }

        ihash = BKDRHash(key);
        if(ihash >= MAX_ENTRIES)
            ihash = ihash / MAX_ENTRIES;

        WaitForSingleObject(mutex, INFINITE);

        if (strcmp(shared_data[ihash].key, key) == 0) {
            flag = 1;
        }

        ReleaseMutex(mutex);

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
            file_mapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            MAX_ENTRIES * sizeof(entry_t)
        );

        if (shared_data == NULL) {
            printf("Failed to map view of file. Error code: %d\n", GetLastError());
            //CloseHandle(file_mapping);
            return ;
        }

        ihash = BKDRHash(key);
        if(ihash >= MAX_ENTRIES)
            ihash = ihash / MAX_ENTRIES;
        
        WaitForSingleObject(mutex, INFINITE);
        strncpy(shared_data[ihash].key, key, MAX_KEY_LEN);
        strncpy(shared_data[ihash].value, value, MAX_VALUE_LEN);
        ReleaseMutex(mutex);

        UnmapViewOfFile(shared_data);
    }

#elif __linux__

    static int shmid;

    void mdb_operate_init()
    {
        printf("[Mdb: Info]: mdb init successfully! \n");
        printf("\n");
    }

    char* mdb_find(char* key)
    {
        return "linux";
    }

    void mdb_insert(char* key, char* value)
    {
        
    }


#endif // __linux__
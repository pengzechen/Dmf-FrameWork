
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

    static HANDLE mdb_mapping;
    static HANDLE mdb_mutex;

    static HANDLE ip_limit_mapping;
    static HANDLE ip_limit_mutex;

    BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpRserved)
    {
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

// 限制ip的共享内存初始话
        ip_limit_mapping = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            MAX_IP_LIMIT_HANDLE * sizeof(IpLimit),
            IP_LIMIT_SHARED_STR
        );
        if (ip_limit_mapping == NULL) {
            printf("Failed to create file mapping object. Error code: %d\n", GetLastError());
            return ;
        }

        ip_limit_mutex = CreateMutex(NULL, FALSE, IP_LIMIT_MUTEX);
        if (ip_limit_mutex == NULL) {
            printf("Failed to create mutex. Error code: %d\n", GetLastError());
            // UnmapViewOfFile(shared_data);
            CloseHandle(ip_limit_mapping);
            return ;
        }

        IpLimit* shared_data = (IpLimit *)MapViewOfFile(
            ip_limit_mapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            MAX_IP_LIMIT_HANDLE * sizeof(IpLimit)
        );

        for(int i=0; i<MAX_IP_LIMIT_HANDLE; i++)
        {
            shared_data[i].time_start = 0;
            shared_data[i].request_num = 0;
            shared_data[i].ab_ban = 0;
            shared_data[i].invalid_num = 0;
            memset(shared_data[i].ip, 0, 16);
        }

        printf("ip_limit_shared_memory size: %d\n", MAX_IP_LIMIT_HANDLE * sizeof(IpLimit));
        printf("mdb_mapping size: %d\n", MAX_ENTRIES * sizeof(entry_t));

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

    int ip_check_valid(char* ip) 
    {
        IpLimit *shared_data;
        unsigned int ihash;
        time_t t = time(NULL);
        int curr_time = time(&t);

        int flag = 0;

        shared_data = (IpLimit *)MapViewOfFile(
            ip_limit_mapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            MAX_IP_LIMIT_HANDLE * sizeof(IpLimit)
        );

        if (shared_data == NULL) {
            printf("Failed to map view of file. Error code: %d\n", GetLastError());
            //CloseHandle(ip_limit_mapping);
            return -2;
        }

        ihash = BKDRHash(ip);
        if(ihash > MAX_IP_LIMIT_HANDLE)
            ihash = ihash % MAX_IP_LIMIT_HANDLE;

        WaitForSingleObject(ip_limit_mutex, INFINITE);
        
        strncpy(shared_data[ihash].ip, ip, 16);

            if(shared_data[ihash].ab_ban) {
                // 被服务器自动禁止的ip
                ReleaseMutex(ip_limit_mutex);
                UnmapViewOfFile(shared_data);
                return 3;
            }

            if ( curr_time - shared_data[ihash].time_start >= 60) {
                shared_data[ihash].time_start = curr_time;
                shared_data[ihash].request_num = 1;   // 超时就归零
            } else {

                if( shared_data[ihash].request_num >= 1000) {  // 请求次数太多

                    shared_data[ihash].time_start = curr_time; // 归零时间
                    shared_data[ihash].request_num = 1;    // 归零次数
                    shared_data[ihash].invalid_num ++;      // 记录异常次数

                    if(shared_data[ihash].invalid_num >= 5)  // 异常数大于2自动禁止
                        shared_data[ihash].ab_ban = 0;      // 不启用限制
                        
                    flag = 2;  // unnomal
                } else {
                    shared_data[ihash].request_num ++;
                }
            }


        ReleaseMutex(ip_limit_mutex);
        UnmapViewOfFile(shared_data);

        return flag;
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
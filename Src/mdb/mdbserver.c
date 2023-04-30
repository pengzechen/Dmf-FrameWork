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

#include <dmfserver/mdb/mdb.h>


#ifdef __WIN32__
int server_start()
{
    HANDLE file_mapping = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        MAX_ENTRIES * sizeof(entry_t),
        SHARED_STR
    );

    if (file_mapping == NULL) {
        printf("Failed to create file mapping. Error code: %d\n", GetLastError());
        return 1;
    }

    entry_t* shared_data = (entry_t*)MapViewOfFile(
        file_mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        MAX_ENTRIES * sizeof(entry_t)
    );

    if (shared_data == NULL) {
        printf("Failed to map view of file. Error code: %d\n", GetLastError());
        CloseHandle(file_mapping);
        return 1;
    }

    HANDLE mutex = CreateMutex(NULL, FALSE, SHARED_MUTEX);
    if (mutex == NULL) {
        printf("Failed to create mutex. Error code: %d\n", GetLastError());
        UnmapViewOfFile(shared_data);
        CloseHandle(file_mapping);
        return 1;
    }

    strcpy(shared_data[0].key, "123");
    strcpy(shared_data[0].value, "test mdb");

    int count = 1;
    while (1) {
        char key[MAX_KEY_LEN];
        char value[MAX_VALUE_LEN];

        printf("Enter key and value separated by a space (or q to quit): ");
        if (fgets(key, MAX_KEY_LEN, stdin) == NULL) {
            break;
        }

        if (strcmp(key, "q\n") == 0) {
            break;
        }

        if (fgets(value, MAX_VALUE_LEN, stdin) == NULL) {
            break;
        }

        // Remove newline character from value string
        value[strcspn(value, "\n")] = '\0';

        WaitForSingleObject(mutex, INFINITE);

        if (count >= MAX_ENTRIES) {
            printf("Database full. Cannot add more entries.\n");
            ReleaseMutex(mutex);
            continue;
        }

        strcpy(shared_data[count].key, key);
        strcpy(shared_data[count].value, value);
        count++;

        ReleaseMutex(mutex);
    }

    CloseHandle(mutex);
    UnmapViewOfFile(shared_data);
    CloseHandle(file_mapping);
}
#endif // WIN32

int main() {

#ifdef __WIN32__
    
#endif // WIN32


    return 0;
}

#include <dmfserver/mdb/mdb.h>


static HANDLE file_mapping;
static HANDLE mutex;
entry_t *shared_data;

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
        UnmapViewOfFile(shared_data);
        CloseHandle(file_mapping);
        return ;
    }
}

char* mdb_find(char* key)
{

    shared_data = (entry_t *)MapViewOfFile(
        file_mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        MAX_ENTRIES * sizeof(entry_t)
    );

    if (shared_data == NULL) {
        printf("Failed to map view of file. Error code: %d\n", GetLastError());
        CloseHandle(file_mapping);
        return NULL;
    }


    WaitForSingleObject(mutex, INFINITE);
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (strcmp(shared_data[i].key, key) == 0) {
            ReleaseMutex(mutex);
            return shared_data[i].value ;
        }
    }

    return "";
    
}
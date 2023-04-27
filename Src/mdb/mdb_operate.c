#include <dmfserver/mdb/mdb.h>

void mdb_find(char* key2)
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
        printf("Failed to create file mapping object. Error code: %d\n", GetLastError());
        return ;
    }

    entry_t *shared_data = (entry_t *)MapViewOfFile(
        file_mapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        MAX_ENTRIES * sizeof(entry_t)
    );

    if (shared_data == NULL) {
        printf("Failed to map view of file. Error code: %d\n", GetLastError());
        CloseHandle(file_mapping);
        return ;
    }

    HANDLE mutex = CreateMutex(NULL, FALSE, SHARED_MUTEX);
    if (mutex == NULL) {
        printf("Failed to create mutex. Error code: %d\n", GetLastError());
        UnmapViewOfFile(shared_data);
        CloseHandle(file_mapping);
        return ;
    }

    char key[MAX_KEY_LEN];
    printf("Enter key to retrieve value (or q to quit): ");
    if (fgets(key, MAX_KEY_LEN, stdin) == NULL) {
    }
    if (strcmp(key, "q\n") == 0) {
    }
    WaitForSingleObject(mutex, INFINITE);
    int found = 0;
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (strcmp(shared_data[i].key, key) == 0) {
            printf("%s\n", shared_data[i].value);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("No entry found for key %s\n", key);
    }
    ReleaseMutex(mutex);
}
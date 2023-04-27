#include <dmfserver/mdb/mdb.h>


int main() {
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

    int count = 0;
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

    return 0;
}

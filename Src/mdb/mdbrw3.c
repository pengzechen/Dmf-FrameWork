#include <dmfserver/mdb/mdb.h>


int main(int argc, char **argv) {
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
        return 1;
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
        return 1;
    }

    HANDLE mutex = CreateMutex(NULL, FALSE, SHARED_MUTEX);
    if (mutex == NULL) {
        printf("Failed to create mutex. Error code: %d\n", GetLastError());
        UnmapViewOfFile(shared_data);
        CloseHandle(file_mapping);
        return 1;
    }

    while (1) {
        printf("Enter 0 to retrieve value, 1 to insert value (or q to quit): ");
        char input[MAX_VALUE_LEN];
        if (fgets(input, MAX_VALUE_LEN, stdin) == NULL) {
            break;
        }

        if (strcmp(input, "q\n") == 0) {
            break;
        }

        int op = atoi(input);
        if (op == 0) {
            // Query operation
            char key[MAX_KEY_LEN];

            printf("Enter key to retrieve value (or q to quit): ");
            if (fgets(key, MAX_KEY_LEN, stdin) == NULL) {
                break;
            }

            if (strcmp(key, "q\n") == 0) {
                break;
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
        } else if (op == 1) {
            // Insert operation
            char key[MAX_KEY_LEN];
            char value[MAX_VALUE_LEN];

            printf("Enter key: ");
            if (fgets(key, MAX_KEY_LEN, stdin) == NULL) {
                break;
            }

            printf("Enter value: ");
            if (fgets(value, MAX_VALUE_LEN, stdin) == NULL) {
                break;
            }

            WaitForSingleObject(mutex, INFINITE);

            int inserted = 0;
            for (int i = 0; i < MAX_ENTRIES; i++) {
                if (strlen(shared_data[i].key) == 0) {
                    strncpy(shared_data[i].key, key, MAX_KEY_LEN);
                    strncpy(shared_data[i].value, value, MAX_VALUE_LEN);
                    inserted = 1;
                    break;
                }
            }

            if (inserted) {
                printf("Inserted value with key %s\n", key);
            } else {
                printf("No space for new entry\n");
			}
			ReleaseMutex(mutex);
			
		} else {
			printf("Invalid input\n");
		}
	}

	CloseHandle(mutex);
	UnmapViewOfFile(shared_data);
	CloseHandle(file_mapping);

	return 0;
}
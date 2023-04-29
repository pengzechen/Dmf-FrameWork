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

int main(int argc, char **argv) {
    
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

            printf("%s\n", mdb_find(key));
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

            mdb_insert(key, value);
			
		} else {
			printf("Invalid input\n");
		}
	}

	return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Global data storage array
unsigned int data[100];

// Function to clear stdin buffer
void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Function to get an unsigned number from user input
unsigned int get_unum(void) {
    unsigned int num;
    printf("Number: ");
    fflush(stdout);
    scanf("%u", &num);
    clear_stdin();
    return num;
}

// Function to store a number in the data storage
void store_number(void) {
    unsigned int index;
    unsigned int number;
    
    printf("Index: ");
    fflush(stdout);
    scanf("%u", &index);
    clear_stdin();
    
    // Check if index is within bounds
    if (index >= 100) {
        printf("*** ERROR! ***\n");
        return;
    }
    
    // Check if index is in the reserved range (indices 10, 11, 12, 13, 14, 15 are reserved for wil)
    if (index % 3 == 0 || index >= 10 && index <= 15) {
        printf("*** ERROR! ***\n");
        printf(" This index is reserved for wil!\n");
        return;
    }
    
    number = get_unum();
    data[index] = number;
}

// Function to read a number from the data storage
void read_number(void) {
    unsigned int index;
    
    printf("Index: ");
    fflush(stdout);
    scanf("%u", &index);
    clear_stdin();
    
    // Check if index is within bounds
    if (index >= 100) {
        printf("*** ERROR! ***\n");
        return;
    }
    
    printf("Number at data[%u] is %u\n", index, data[index]);
}

// Program timeout/alarm handler setup
void prog_timeout(void) {
    // Intentionally empty or minimal implementation
    // This function is called but does nothing in the original binary
}

int main(int argc, char **argv) {
    char command[64];
    int quit = 0;
    
    // Initialize data array to zero
    memset(data, 0, sizeof(data));
    
    // Reserve some initial values for wil (indices 10-15)
    data[10] = 0;
    data[11] = 0;
    data[12] = 0;
    data[13] = 0;
    data[14] = 0;
    data[15] = 0;
    
    // Print welcome banner
    printf("----------------------------------------------------\n");
    printf("Welcome to wil's crappy number storage service!\n");
    printf("----------------------------------------------------\n");
    printf("Commands:\n");
    printf("    store - store a number into the data storage\n");
    printf("    read  - read a number from the data storage\n");
    printf("    quit  - exit the program\n");
    printf("----------------------------------------------------\n");
    printf(" wil has reserved some storage :>\n");
    printf("----------------------------------------------------\n");
    
    // Main command loop
    while (!quit) {
        printf("Input command: ");
        fflush(stdout);
        
        // Read command from stdin
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(command);
        if (len > 0 && command[len-1] == '\n') {
            command[len-1] = '\0';
        }
        
        // Process command
        if (strcmp(command, "store") == 0) {
            store_number();
            if (data[0] != 0) { // Check for some error condition
                printf("Failed to do %s command\n", command);
            } else {
                printf("Completed %s command successfully\n", command);
            }
        } else if (strcmp(command, "read") == 0) {
            read_number();
            printf("Completed %s command successfully\n", command);
        } else if (strcmp(command, "quit") == 0) {
            quit = 1;
        } else {
            // Unknown command
            printf("Failed to do %s command\n", command);
        }
    }
    
    return 0;
}

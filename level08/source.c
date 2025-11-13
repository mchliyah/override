#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to write log messages
void log_wrapper(char *message) {
    FILE *log_file;
    
    // Open log file in append mode
    log_file = fopen("./backups/.log", "w");
    
    if (log_file == NULL) {
        return;
    }
    
    // Write log message
    fprintf(log_file, "LOG: %s\n", message);
    
    // Close log file
    fclose(log_file);
}

// Main function to backup files
int main(int argc, char *argv[]) {
    FILE *source_file;
    FILE *backup_file;
    char backup_path[512];
    char log_message[512];
    char filename_copy[256];
    int ch;
    
    // Check if filename argument is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }
    
    // Copy filename for processing
    strcpy(filename_copy, argv[1]);
    
    // Remove any path components, keep only filename
    // Find the last occurrence of newline or path separator
    size_t len = strcspn(filename_copy, "\n");
    if (len < strlen(filename_copy)) {
        filename_copy[len] = '\0';
    }
    
    // Open source file for reading
    source_file = fopen(argv[1], "r");
    if (source_file == NULL) {
        fprintf(stderr, "ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }
    
    // Create log message for starting backup
    snprintf(log_message, sizeof(log_message), "Starting back up: ");
    strncat(log_message, argv[1], sizeof(log_message) - strlen(log_message) - 1);
    log_wrapper(log_message);
    
    // Construct backup file path
    strcpy(backup_path, "./backups/");
    strncat(backup_path, argv[1], sizeof(backup_path) - strlen(backup_path) - 1);
    
    // Open backup file for writing
    backup_file = fopen(backup_path, "w");
    if (backup_file == NULL) {
        fprintf(stderr, "ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
        fclose(source_file);
        exit(1);
    }
    
    // Copy file contents character by character
    while ((ch = fgetc(source_file)) != EOF) {
        fputc(ch, backup_file);
    }
    
    // Close both files
    fclose(source_file);
    fclose(backup_file);
    
    // Create log message for finished backup
    snprintf(log_message, sizeof(log_message), "Finished back up ");
    strncat(log_message, argv[1], sizeof(log_message) - strlen(log_message) - 1);
    log_wrapper(log_message);
    
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    char password_file_contents[100];
    char input_username[100];
    char input_password[100];
    FILE *password_file;
    size_t bytes_read;
    int result;
    
    // Initialize buffers
    memset(password_file_contents, 0, sizeof(password_file_contents));
    memset(input_username, 0, sizeof(input_username));
    memset(input_password, 0, sizeof(input_password));
    
    // Open the password file
    password_file = fopen("/home/users/level03/.pass", "r");
    
    if (password_file == NULL)
    {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }
    
    // Read password from file
    bytes_read = fread(password_file_contents, 1, 41, password_file);
    
    // Remove newline from password
    password_file_contents[strcspn(password_file_contents, "\n")] = '\0';
    
    // Close the password file
    fclose(password_file);
    
    // Print the banner
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");
    
    // Prompt for username
    printf("--[ Username: ");
    fgets(input_username, 100, stdin);
    input_username[strcspn(input_username, "\n")] = '\0';
    
    // Prompt for password
    printf("--[ Password: ");
    fgets(input_password, 100, stdin);
    input_password[strcspn(input_password, "\n")] = '\0';
    
    // Print separator
    puts("*****************************************");
    
    // Compare the entered password with the stored password
    result = strncmp(password_file_contents, input_password, 41);
    
    if (result == 0)
    {
        // Password matches - grant access
        printf("Greetings, %s!\n", input_username);
        system("/bin/sh");
    }
    else
    {
        // Password does not match - deny access
        printf("%s does not have access!\n", input_username);
        exit(1);
    }
    
    return 0;
}

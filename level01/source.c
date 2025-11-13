#include <stdio.h>
#include <string.h>

// Function prototypes
void a_user_name(void);
int verify_user_name(char *username);
int verify_user_pass(char *password);

// Main function
int main(void)
{
    char username[100];
    char password[100];
    
    // Print admin login prompt
    puts("********* ADMIN LOGIN PROMPT *********");
    
    // Print verifying username message
    a_user_name();
    
    // Prompt for username
    printf("Enter Username: ");
    
    // Read username from stdin
    fgets(username, 100, stdin);
    
    // Verify username
    if (verify_user_name(username) == 0)
    {
        puts("nope, incorrect username...");
        return 1;
    }
    
    // Prompt for password
    printf("Enter Password: ");
    
    // Read password from stdin
    fgets(password, 100, stdin);
    
    // Verify password
    if (verify_user_pass(password) == 0)
    {
        puts("nope, incorrect password...");
        return 1;
    }
    
    return 0;
}

// Function to print verification message
void a_user_name(void)
{
    puts("verifying username.....");
}

// Function to verify username
int verify_user_name(char *username)
{
    // Remove newline character if present
    size_t len = strlen(username);
    if (len > 0 && username[len - 1] == '\n')
    {
        username[len - 1] = '\0';
    }
    
    // Compare with expected username "dat_wil"
    if (strcmp(username, "dat_wil") == 0)
    {
        return 1;
    }
    
    return 0;
}

// Function to verify password
int verify_user_pass(char *password)
{
    // Remove newline character if present
    size_t len = strlen(password);
    if (len > 0 && password[len - 1] == '\n')
    {
        password[len - 1] = '\0';
    }
    
    // Compare with expected password "admin"
    if (strcmp(password, "admin") == 0)
    {
        return 1;
    }
    
    return 0;
}

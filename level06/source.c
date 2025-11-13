#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ptrace.h>

// Function prototypes
void clear_stdin(void);
unsigned int get_unum(void);
void prog_timeout(int sig);
void enable_timeout_cons(void);
int auth(char *login, unsigned int serial);
int main(void);

/**
 * clear_stdin - Clears the standard input buffer
 */
void clear_stdin(void)
{
    int c;
    
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

/**
 * get_unum - Gets an unsigned integer from user input
 * Return: The unsigned integer entered by the user
 */
unsigned int get_unum(void)
{
    unsigned int num;
    
    fflush(stdout);
    scanf("%u", &num);
    clear_stdin();
    
    return num;
}

/**
 * prog_timeout - Signal handler for alarm timeout
 * @sig: Signal number
 */
void prog_timeout(int sig)
{
    (void)sig;
    exit(1);
}

/**
 * enable_timeout_cons - Enables a 60-second timeout
 */
void enable_timeout_cons(void)
{
    signal(SIGALRM, prog_timeout);
    alarm(60);
}

/**
 * auth - Authenticates user based on login and serial
 * @login: The login string
 * @serial: The serial number to validate
 * 
 * Return: 0 if authenticated, 5 otherwise
 */
int auth(char *login, unsigned int serial)
{
    size_t login_len;
    unsigned int hash;
    unsigned int i;
    
    // Remove newline from login
    login[strcspn(login, "\n")] = 0;
    
    // Get length of login (max 32 characters)
    login_len = strnlen(login, 32);
    
    // Login must be less than 6 characters
    if (login_len <= 5)
    {
        return 1;
    }
    
    // Check for ptrace (anti-debugging)
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
    {
        puts("\x1b[32m.---------------------------."); 
        puts("\x1b[31m| !! TAMPERING DETECTED !!  |");
        puts("\x1b[32m'---------------------------'");
        return 1;
    }
    
    // Calculate hash from login
    hash = (login[3] ^ 0x1337) + 0x5eeded;
    
    for (i = 0; i < login_len; i++)
    {
        if (login[i] <= 31)
        {
            return 1;
        }
        
        hash += (hash ^ (unsigned int)login[i]) % 0x539;
    }
    
    // Check if serial matches calculated hash
    if (serial == hash)
    {
        return 0;
    }
    
    return 1;
}

/**
 * main - Entry point
 * 
 * Return: 0 on success
 */
int main(void)
{
    char login[32];
    unsigned int serial;
    int auth_result;
    
    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    
    printf("-> Enter Login: ");
    fgets(login, 32, stdin);
    
    puts("***** NEW ACCOUNT DETECTED ********");
    
    printf("-> Enter Serial: ");
    serial = get_unum();
    
    auth_result = auth(login, serial);
    
    if (auth_result == 0)
    {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }
    
    return 1;
}

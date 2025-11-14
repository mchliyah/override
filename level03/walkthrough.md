# Level03

## Binary Analysis

`level03` is a SUID binary owned by `level04`. The program prompts for a password and validates it against a predictable algorithm that uses a hardcoded constant and a switch/jump table.

Key facts:
- Hardcoded value: `0x1337d00d` (decimal: 322424845)
- Validation uses the expression: `result = 0x1337d00d - password` and a switch based on `result`.

## Vulnerability

From the disassembly the check looks like:

```asm
	movl $0x1337d00d, 0x4(%esp)    ; push hardcoded value
	mov %eax,(%esp)                ; push user input
	call test                      ; test(input, 0x1337d00d)

; inside test:
	sub %eax,%ecx                  ; ecx = 0x1337d00d - input
	cmpl $0x15,-0xc(%ebp)          ; compare result with 21
	ja   fail_case                 ; if result > 21 -> fail
	mov -0xc(%ebp),%eax
	shl $0x2,%eax                  ; index * 4
	add $0x80489f0,%eax            ; jump table base
	jmp *%eax                      ; jump to case
```

The check effectively requires `0x1337d00d - password <= 21` (and >= 0), so valid passwords are in a small, contiguous range.

## Exploitation

Solve the inequality:

- password >= 322424845 - 21 = 322424824
- password <= 322424845

Any value in that range is accepted. For example, `322424827` works.

Run the program and provide the password:

```bash
./level03
Password: 322424827
```

Once the check passes, the binary grants a shell as the next privilege level. Retrieve the flag/password for `level04`:

```bash
cat /home/users/level04/.pass
# kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```

## Key Learning

- Read the disassembly to extract numeric constraints rather than relying on brute force.
- Switch/jump-table based checks often reduce the valid input set to a small range.
- SUID binaries with predictable checks can be escalated when validation is weak.

## Result

Level04 password: `kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf`

